#if defined(ARDUINO_ESP32_DEV) // Only for ESP32

#include "Controller.hpp"

Controller::Controller()
{

}

void Controller::init()
{
    RS485::getInstance().init(CONTROLLER_PIN_RX, CONTROLLER_PIN_TX, CONTROLLER_PIN_DE_RE);

    Serial.begin(115200);
    Serial.println("[Controller::init] Init start");

    UI::getInstance().init();
    Battery::getInstance().init();

    PS4.begin("e0:d4:e8:72:14:37");

    comms_mutex_ = xSemaphoreCreateMutex();

    Serial.println("[Controller::init] Init done");

    // Queues to send Structs between tasks
    battery_response_queue_ = xQueueCreate(3 /* Max items */, sizeof(BatteryInformation));
    lights_message_queue_ = xQueueCreate(3 /* Max items */, sizeof(LightsInformation));

    // Show dualshock connection screen
    UI::getInstance().showDualshockConnectionScreen();

    // Wait for dualshock connection
    while (!PS4.isConnected())
    {
        Serial.println("[Controller::init] Waiting for PS4 controller connection...");
        UI::getInstance().refresh();
        delay(1000);
    }

    UI::getInstance().showDashboard();

    PS4.setLed(255, 255, 0); // Yellow submarine
    // PS4.setRumble(255, 255); // Small rumble to notify connection
    // PS4.sendToController();
    // delay(500);
    // PS4.setRumble(0, 0); // Stop rumble
    PS4.sendToController();

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

void Controller::loop()
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

    UI::getInstance().refresh();
}

void Controller::rovControlTask(void* params)
{
    Controller* controller = static_cast<Controller*>(params);

    // Spin at 10Hz
    const TickType_t delay_ticks = pdMS_TO_TICKS(100);
    while (true)
    {
        // Read from DualShock4
        if (!PS4.isConnected())
        {
            Serial.println("[Controller::rovControlTask] PS4 Controller lost!");
            vTaskDelay(delay_ticks);
            continue;
        }

        // Control messages for ROV
        ControlMessage motor_up_msg; motor_up_msg.motor_id = 0; // Motor 1
        ControlMessage motor_down_msg; motor_down_msg.motor_id = 1; // Motor 2
        ControlMessage motor_left_msg; motor_left_msg.motor_id = 2; // Motor 3
        ControlMessage motor_right_msg; motor_right_msg.motor_id = 3; // Motor 4
        LightsMessage lights_msg;

        // PS4 readings
        bool r1_pressed = PS4.R1();
        bool l1_pressed = PS4.L1();

        uint8_t r2_value = PS4.R2Value();

        int8_t left_stick_x = PS4.LStickX();
        int8_t right_stick_y = PS4.RStickY();

        // Left-Right-Forward
        




        motor_left_msg.thrust = left_stick_x < -10 ? -left_stick_x * 2 : 0; // Deadzone of 10 and scale to 0-255

        // Up-Down
        motor_up_msg.thrust = right_stick_y > 10 ? right_stick_y * 2 : 0; // Deadzone of 10 and scale to 0-255
        motor_down_msg.thrust = right_stick_y < -10 ? -right_stick_y * 2 : 0; // Deadzone of 10 and scale to 0-255
        
        // Check lights control
        static bool first_run = true;
        bool intensity_changed = (r1_pressed || l1_pressed || first_run);
        first_run = false;

        if (r1_pressed)
        {
            controller->increaseLightsIntensity();
        }
        else if (l1_pressed)
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

void Controller::controllerBatteryTask(void* params)
{
    Controller* controller = static_cast<Controller*>(params);

    // Spin at 1 seconds
    const TickType_t delay_ticks = pdMS_TO_TICKS(1000);
    while (true)
    {   
        // Controller Box
        BatteryInformation controller_battery_info = Battery::getInstance().info();
        controller_battery_info.type = BatteryType::CONTROLLER;
        
        // DualShock4
        BatteryInformation dualshock_battery_info;
        dualshock_battery_info.type = BatteryType::DUALSHOCK;
        dualshock_battery_info.percentage = PS4.Battery() * 10; // PS4.Battery() returns 0-10
        dualshock_battery_info.charging = PS4.Charging();

        // Add to queue to update UI in main task
        xQueueSend(controller->battery_response_queue_, &controller_battery_info, 0);
        xQueueSend(controller->battery_response_queue_, &dualshock_battery_info, 0);

        vTaskDelay(delay_ticks);
    }
}

void Controller::rovBatteryTask(void* params)
{
    Controller* controller = static_cast<Controller*>(params);

    // Spin at 10 seconds
    const TickType_t delay_ticks = pdMS_TO_TICKS(10000);
    while (true)
    {
        Serial.println("[Controller::rovBatteryTask] Requesting battery...");

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

void Controller::increaseLightsIntensity()
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

void Controller::decreaseLightsIntensity()
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


#endif