#if defined(ARDUINO_ESP32_DEV) // Only for ESP32

#include "GroundStation.hpp"

GroundStation* GroundStation::instance_ = nullptr;

GroundStation::GroundStation()
{

}

void GroundStation::init()
{
    instance_ = this;

    RS485::getInstance().init(CONTROLLER_PIN_RX, CONTROLLER_PIN_TX, CONTROLLER_PIN_DE_RE);

    Serial.begin(115200);
    Serial.println("[GroundStation::init] Init start");

    UI::getInstance().init();
    Battery::getInstance().init();

    // Init gamepad
    BP32.setup(
        &onConnectedController,
        &onDisconnectedController
    );

    comms_mutex_ = xSemaphoreCreateMutex();

    Serial.println("[GroundStation::init] Init done");

    // Queues to send Structs between tasks
    battery_response_queue_ = xQueueCreate(3 /* Max items */, sizeof(BatteryInformation));
    lights_message_queue_ = xQueueCreate(3 /* Max items */, sizeof(LightsInformation));
    motor_message_queue_ = xQueueCreate(8 /* Max items */, sizeof(MotorInformation));

    // Show dualshock connection screen
    UI::getInstance().showGamepadConnectionScreen();

    // Wait for dualshock connection
    while (gamepad_ == nullptr || !gamepad_->isConnected())
    {
        Serial.println("[GroundStation::init] Waiting for controller connection...");
        UI::getInstance().refresh();
        BP32.update();
        delay(1000);
    }

    UI::getInstance().showDashboard();

    //gamepad_->setLed(255, 255, 0); // Yellow submarine

    // Control task to send control messages periodically
    xTaskCreate(
        rovControlTask,
        "RovControlTask",
        4096,
        this, /* Parameter passed as input of the task */
        1,
        &rov_control_task_
    );

    // Battery task to request battery periodically
    xTaskCreate(
        rovBatteryTask,
        "RovBatteryTask",
        4096,
        this, /* Parameter passed as input of the task */
        1,
        &rov_battery_task_
    );

    // Battery task to request battery periodically
    xTaskCreate(
        controllerBatteryTask,
        "ControllerBatteryTask",
        4096,
        this, /* Parameter passed as input of the task */
        1,
        &controller_battery_task_
    );
}

void GroundStation::loop()
{
    // UI updates at 10Hz  
    BatteryInformation battery_info;
    while (xQueueReceive(battery_response_queue_, &battery_info, 0) == pdTRUE)
    {
        UI::getInstance().update(battery_info);
    }

    LightsInformation lights_info;
    while (xQueueReceive(lights_message_queue_, &lights_info, 0) == pdTRUE)
    {
        UI::getInstance().update(lights_info);
    }

    MotorInformation motor_info;
    while (xQueueReceive(motor_message_queue_, &motor_info, 0) == pdTRUE)
    {
        UI::getInstance().update(motor_info);
    }

    UI::getInstance().refresh();
}

void GroundStation::rovControlTask(void* params)
{
    GroundStation* controller = static_cast<GroundStation*>(params);

    // Spin at 10Hz
    const TickType_t delay_ticks = pdMS_TO_TICKS(100);
    while (true)
    {
        // Read from gamepad
        if (!controller->gamepad_->isConnected())
        {
            Serial.println("[GroundStation::rovControlTask] Gamepad lost!");
            BP32.update(); // Update to check if gamepad is reconnected
            vTaskDelay(delay_ticks);
            continue;
        }

        BP32.update(); // Update gamepad state

        // Create control messages for ROV
        ControlMessage motor_up_msg; motor_up_msg.motor_id = 0; // Motor 1
        ControlMessage motor_down_msg; motor_down_msg.motor_id = 1; // Motor 2
        ControlMessage motor_left_msg; motor_left_msg.motor_id = 2; // Motor 3
        ControlMessage motor_right_msg; motor_right_msg.motor_id = 3; // Motor 4
        LightsMessage lights_msg;

        // GroundStation readings
        bool increase_light_pressed = controller->gamepad_->r1();
        bool decrease_light_pressed = controller->gamepad_->l1();

        int yaw_stick = controller->gamepad_->axisX();
        int up_down_stick = - controller->gamepad_->axisY();
        int fordward_stick = - controller->gamepad_->axisRY();

        Serial.printf("[GroundStation::rovControlTask] Joystick readings: Yaw: %d, Up/Down: %d, Forward: %d\n", yaw_stick, up_down_stick, fordward_stick);

        // Truncate to 500
        yaw_stick = std::max(-JOYSTICK_MAX, std::min(yaw_stick, JOYSTICK_MAX));
        up_down_stick = std::max(-JOYSTICK_MAX, std::min(up_down_stick, JOYSTICK_MAX));
        fordward_stick = std::max(-JOYSTICK_MAX, std::min(fordward_stick, JOYSTICK_MAX));

        Serial.printf("[GroundStation::rovControlTask] Clamped joystick readings: Yaw: %d, Up/Down: %d, Forward: %d\n", yaw_stick, up_down_stick, fordward_stick);
        
        bool options = controller->gamepad_->miscButtons() & 0x04;
        bool share   = controller->gamepad_->miscButtons() & 0x02;
        bool force_ota = false;

        // Check force OTA button
        if (force_ota)
        {
            Serial.println("[GroundStation::rovControlTask] OTA button pressed, forcing OTA mode on ROV...");
            OTAUpdateMessage ota_msg;
            uint8_t packed_ota_msg = Protocol::pack(ota_msg);

            xSemaphoreTake(controller->comms_mutex_, portMAX_DELAY); // Adquire mutex
            RS485::getInstance().txMode();
            RS485::getInstance().send(packed_ota_msg);
            xSemaphoreGive(controller->comms_mutex_); // Release mutex

            vTaskDelay(delay_ticks);
            continue; // Skip the rest of the loop to avoid sending other commands
        }

        if (options)
        {
            UI::getInstance().showMotorThurstScreen();
        }

        if (share)
        {
            UI::getInstance().showDashboard();
        }

        // Left-Right-Forward
        motor_left_msg.thrust = 0;
        motor_right_msg.thrust = 0;

        int available_thrust = 255;
        if (yaw_stick > JOYSTICK_DEADZONE)
        {
            // Turning right
            motor_left_msg.thrust = yaw_stick / 2;

            // Compute available thrust
            available_thrust = 255 - motor_left_msg.thrust;
        }
        else if (yaw_stick < -JOYSTICK_DEADZONE)
        {
            // Turning left
            motor_right_msg.thrust = (-yaw_stick) / 2;
            
            // Compute available thrust
            available_thrust = 255 - motor_right_msg.thrust;
        }

        float fordward_percent= static_cast<float>(std::abs(fordward_stick)) / static_cast<float>(JOYSTICK_MAX); // 0.0 to 1.0
        
        motor_left_msg.thrust += available_thrust * fordward_percent;
        motor_right_msg.thrust += available_thrust * fordward_percent;
        
        // Up-Down
        motor_up_msg.thrust = up_down_stick > JOYSTICK_DEADZONE ? up_down_stick / 2 : 0; // Deadzone of JOYSTICK_DEADZONE and scale to 0-255
        motor_down_msg.thrust = up_down_stick < -JOYSTICK_DEADZONE ? -up_down_stick / 2 : 0; // Deadzone of JOYSTICK_DEADZONE and scale to 0-255
        
        // Update UI with what robot receives (unpacking)
        MotorInformation motor_info;
        motor_info.motor_id = 0; // Motor 1
        motor_info.thrust = motor_up_msg.thrust;
        xQueueSend(controller->motor_message_queue_, &motor_info, 0);

        motor_info.motor_id = 1; // Motor 2
        motor_info.thrust = motor_down_msg.thrust;
        xQueueSend(controller->motor_message_queue_, &motor_info, 0);

        motor_info.motor_id = 2; // Motor 3
        motor_info.thrust = motor_left_msg.thrust;
        xQueueSend(controller->motor_message_queue_, &motor_info, 0);

        motor_info.motor_id = 3; // Motor 4
        motor_info.thrust = motor_right_msg.thrust;
        xQueueSend(controller->motor_message_queue_, &motor_info, 0);

        // Print for debug
        Serial.printf("[GroundStation::rovControlTask] Motor thrusts: Up: %d, Down: %d, Left: %d, Right: %d\n", motor_up_msg.thrust, motor_down_msg.thrust, motor_left_msg.thrust, motor_right_msg.thrust);


        // Check lights control
        static bool first_run = true;
        bool intensity_changed = (increase_light_pressed || decrease_light_pressed || first_run);
        first_run = false;

        if (increase_light_pressed)
        {
            controller->increaseLightsIntensity();
        }
        else if (decrease_light_pressed)
        {
            controller->decreaseLightsIntensity();
        }

        lights_msg.intensity = controller->lights_intensity_;

        if (intensity_changed)
        {
            // Update UI with what robot receives (unpacking)
            LightsInformation lights_info;
            lights_info.intensity = controller->lights_intensity_;
            xQueueSend(controller->lights_message_queue_, &lights_info, 0);
        }

        // Pack messages and send to ROV
        uint8_t packed_msg_up = Protocol::pack(motor_up_msg);
        uint8_t packed_msg_down = Protocol::pack(motor_down_msg);
        uint8_t packed_msg_left = Protocol::pack(motor_left_msg);
        uint8_t packed_msg_right = Protocol::pack(motor_right_msg);
     
        // Send data
        xSemaphoreTake(controller->comms_mutex_, portMAX_DELAY); // Adquire mutex
        RS485::getInstance().txMode();
        RS485::getInstance().send(packed_msg_up);
        RS485::getInstance().send(packed_msg_down);
        RS485::getInstance().send(packed_msg_left);
        RS485::getInstance().send(packed_msg_right);
        
        // Pack lights message and send to ROV if intensity changed
        if (intensity_changed)
        {
            uint8_t packed_lights_msg = Protocol::pack(lights_msg);
            RS485::getInstance().send(packed_lights_msg);
        }

        xSemaphoreGive(controller->comms_mutex_); // Release mutex

        vTaskDelay(delay_ticks);
    }
}

void GroundStation::controllerBatteryTask(void* params)
{
    GroundStation* controller = static_cast<GroundStation*>(params);

    // Spin at 1 seconds
    const TickType_t delay_ticks = pdMS_TO_TICKS(1000);
    while (true)
    {   
        // GroundStation Box
        BatteryInformation controller_battery_info = Battery::getInstance().info();
        controller_battery_info.type = BatteryType::CONTROLLER;
        
        // DualShock4
        BatteryInformation dualshock_battery_info;
        dualshock_battery_info.type = BatteryType::DUALSHOCK;
        dualshock_battery_info.percentage = controller->gamepad_->battery();
        dualshock_battery_info.charging = false; //controller->gamepad_->isCharging();

        // Add to queue to update UI in main task
        xQueueSend(controller->battery_response_queue_, &controller_battery_info, 0);
        xQueueSend(controller->battery_response_queue_, &dualshock_battery_info, 0);

        vTaskDelay(delay_ticks);
    }
}

void GroundStation::rovBatteryTask(void* params)
{
    GroundStation* controller = static_cast<GroundStation*>(params);

    // Spin at 10 seconds
    const TickType_t delay_ticks = pdMS_TO_TICKS(10000);
    while (true)
    {
        Serial.println("[GroundStation::rovBatteryTask] Requesting battery...");

        BatteryRequestMessage request;
        uint8_t packed_request = Protocol::pack(request);

        // Send request
        xSemaphoreTake(controller->comms_mutex_, portMAX_DELAY); // Adquire mutex
        RS485::getInstance().txMode();
        RS485::getInstance().send(packed_request);
        RS485::getInstance().halfWait(); // Ensure data is sent before changing Channel mode

        RS485::getInstance().rxMode(); // Switch to rx mode to receive response
        RS485::getInstance().wait(); // Give time to switch and receive
        int received_response = RS485::getInstance().readLast(); // Receive last byte

        xSemaphoreGive(controller->comms_mutex_); // Release mutex

        BatteryInformation rov_battery_info;
        rov_battery_info.type = BatteryType::ROV;
        rov_battery_info.percentage = 0;
        rov_battery_info.voltage = 0.0f;

        if (received_response == -1)
        {
            // Error
            Serial.println("[ERROR] No response on BatteryResquestMessage");
        }
        else
        {
            BatteryResponseMessage response;
            if (!Protocol::unpack(static_cast<uint8_t>(received_response), response))
            {
                // Error, other thing received
                Serial.println("[ERROR] Invalid response on BatteryResquestMessage");
            }
            else
            {
                rov_battery_info.percentage = response.percentage;
                rov_battery_info.voltage = Battery::getInstance().convertPercentageToVoltage(response.percentage);  
                Serial.println("[INFO] Received battery from ROV");
                
                // Add to queue to update UI in main task
                xQueueSend(controller->battery_response_queue_, &rov_battery_info, 0);

            }
        }

        vTaskDelay(delay_ticks);
    }
}

void GroundStation::increaseLightsIntensity()
{
    if (lights_intensity_ <= 245)
    {
        lights_intensity_ += 10;
    }
    else
    {
        lights_intensity_ = 255;
    }
}

void GroundStation::decreaseLightsIntensity()
{
    if (lights_intensity_ >= 10)
    {
        lights_intensity_ -= 10;
    }
    else
    {
        lights_intensity_ = 0;
    }
}

void GroundStation::onConnectedController(ControllerPtr ctl)
{
    if (instance_ == nullptr)
    {
        return;
    }

    instance_->gamepad_ = ctl;
    Serial.println("[GroundStation::onConnectedController] GroundStation connected");
}

void GroundStation::onDisconnectedController(ControllerPtr ctl)
{
    if (instance_ == nullptr)
    {
        return;
    }

    if (instance_->gamepad_ == ctl)
    {
        instance_->gamepad_ = nullptr;
    }

    Serial.println("[GroundStation::onDisconnectedController] GroundStation disconnected");
}


#endif