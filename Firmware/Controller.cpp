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
    PS4.setRumble(255, 255); // Small rumble to notify connection
    PS4.sendToController();
    delay(500);
    PS4.setRumble(0, 0); // Stop rumble
    PS4.sendToController();

    // Control task to send control messages periodically
    xTaskCreate(
        rovControlTask,
        "RovControlTask",
        4096,
        this, /* Parameter passed as input of the task */
        1,
        nullptr
    );

    // Battery task to request battery periodically
    xTaskCreate(
        rovBatteryTask,
        "RovBatteryTask",
        4096,
        this, /* Parameter passed as input of the task */
        1,
        nullptr
    );

    // Battery task to request battery periodically
    xTaskCreate(
        controllerBatteryTask,
        "ControllerBatteryTask",
        4096,
        this, /* Parameter passed as input of the task */
        1,
        nullptr
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

        int8_t left_stick_y = PS4.LStickY();

        bool r1_pressed = PS4.R1();
        bool l1_pressed = PS4.L1();

        static bool first_run = true;

        // Check lights control
        bool intensity_changed = (r1_pressed || l1_pressed || first_run);
        first_run = false;

        if (r1_pressed)
        {
            // Increment
            if (controller->lights_intensity_ <= 245)
            {
                controller->lights_intensity_ += 10;
            }
            else
            {
                controller->lights_intensity_ = 255;
            }
        }
        else if (l1_pressed)
        {
            // Decrement
            if (controller->lights_intensity_ >= 10)
            {
                controller->lights_intensity_ -= 10;
            }
            else
            {
                controller->lights_intensity_ = 0;
            }
        }

        if (intensity_changed)
        {
            // Send lights message
            LightsMessage lights_msg;
            lights_msg.intensity = controller->lights_intensity_;
            uint8_t packed_lights_msg = Protocol::pack(lights_msg);

            xSemaphoreTake(controller->comms_mutex_, portMAX_DELAY); // Adquire mutex
            RS485::getInstance().txMode();
            RS485::getInstance().send(packed_lights_msg);
            xSemaphoreGive(controller->comms_mutex_); // Release mutex

            // Update UI with what robot receives (unpacking)
            LightsMessage received_lights_msg;
            Protocol::unpack(packed_lights_msg, received_lights_msg);
            LightsInformation lights_info;
            lights_info.intensity = received_lights_msg.intensity;
            xQueueSend(controller->lights_message_queue_, &lights_info, 0);
        }

        ControlMessage original_control_msg_up;
        original_control_msg_up.motor_id = 0; // Motor 1
        original_control_msg_up.thrust = left_stick_y > 10 ? left_stick_y * 2 : 0;

        uint8_t packed_msg_up = Protocol::pack(original_control_msg_up);
     
        // Send data
        xSemaphoreTake(controller->comms_mutex_, portMAX_DELAY); // Adquire mutex
        RS485::getInstance().txMode();
        RS485::getInstance().send(packed_msg_up);
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

        if (received_response == -1)
        {
            // Error
            rov_battery_info.percentage = 0;
            Serial.println("[ERROR] No response on BatteryResquestMessage");
        }
        else
        {
            BatteryResponseMessage response;
            if (!Protocol::unpack(static_cast<uint8_t>(received_response), response))
            {
                // Error, other thing received
                rov_battery_info.percentage = 0;
                Serial.print("[ERROR] Invalid response on BatteryResquestMessage");
            }
            else
            {
                rov_battery_info.percentage = response.percentage;
                Serial.println("[INFO] Received battery from ROV");
                
                // Add to queue to update UI in main task
                xQueueSend(controller->battery_response_queue_, &rov_battery_info, 0);

            }
        }

        vTaskDelay(delay_ticks);
    }
}


#endif