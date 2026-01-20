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

    PS4.begin("e0:d4:e8:72:14:37");

    comms_mutex_ = xSemaphoreCreateMutex();

    Serial.println("[Controller::init] Init done");

    // Queues to send Structs between tasks
    battery_response_queue_ = xQueueCreate(3 /* Max items */, sizeof(BatteryInformation));
    control_message_queue_ = xQueueCreate(8 /* Max items */, sizeof(MotorInformation));

    // Wait for dualshock connection
    while (!PS4.isConnected())
    {
        Serial.println("[Controller::init] Waiting for PS4 controller connection...");
        delay(1000);
    }

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
    MotorInformation motor_info;
    while (xQueueReceive(control_message_queue_, &motor_info, 0) == pdTRUE)
    {
        UI::getInstance().update(motor_info);
    }

    BatteryInformation battery_info;
    while (xQueueReceive(battery_response_queue_, &battery_info, 0) == pdTRUE)
    {
        UI::getInstance().update(battery_info);
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

        ControlMessage original_control_msg_up;
        original_control_msg_up.motor_id = 0; // Motor 1
        original_control_msg_up.thrust = left_stick_y > 10 ? left_stick_y * 2 : 0;

        uint8_t packed_msg_up = Protocol::pack(original_control_msg_up);
     
        // Send data
        xSemaphoreTake(controller->comms_mutex_, portMAX_DELAY); // Adquire mutex
        RS485::getInstance().txMode();
        RS485::getInstance().send(packed_msg_up);
        xSemaphoreGive(controller->comms_mutex_); // Release mutex

        // Update UI with what robot receives (unpacking)
        ControlMessage received_control_msg_up;
        Protocol::unpack(packed_msg_up, received_control_msg_up);
        
        MotorInformation motor_info_up;
        motor_info_up.motor_id = received_control_msg_up.motor_id;
        motor_info_up.thrust = received_control_msg_up.thrust;

        // Add to queue to update UI in main task
        xQueueSend(controller->control_message_queue_, &motor_info_up, 0);

        vTaskDelay(delay_ticks);
    }
}

void Controller::controllerBatteryTask(void* params)
{
    Controller* controller = static_cast<Controller*>(params);

    // Spin at 30 seconds
    const TickType_t delay_ticks = pdMS_TO_TICKS(30000);
    while (true)
    {
        BatteryInformation controller_battery_info;
        controller_battery_info.type = BatteryType::CONTROLLER;
        controller_battery_info.percentage = 75; // Dummy value

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
            }
        }

        // Add to queue to update UI in main task
        xQueueSend(controller->battery_response_queue_, &rov_battery_info, 0);

        vTaskDelay(delay_ticks);
    }
}
/*
void Controller::loop()
{
    static int iterations = 0;

    iterations++; 

    if (!PS4.isConnected())
    {
        Serial.println("[Controller::init] PS4 Controller lost!");
    }
    else
    {
        int8_t left_stick_x = PS4.LStickX();
        int8_t left_stick_y = PS4.LStickY();
        uint8_t r2_value = PS4.R2Value();

        // What i read
        ControlMessage original_control_msg_up;
        original_control_msg_up.motor_id = 0; // Motor 1
        original_control_msg_up.thrust = left_stick_y > 10 ? left_stick_y * 2 : 0;

        ControlMessage original_control_msg_down;
        original_control_msg_down.motor_id = 1; // Motor 2
        original_control_msg_down.thrust = left_stick_y < -10 ? (-left_stick_y) * 2 : 0;
        
        ControlMessage original_control_msg_left;
        original_control_msg_left.motor_id = 2; // Motor 3
        original_control_msg_left.thrust = left_stick_x < -10 ? (-left_stick_x) * 2 : 0;

        ControlMessage original_control_msg_right;
        original_control_msg_right.motor_id = 3; // Motor 4
        original_control_msg_right.thrust = left_stick_x > 10 ? left_stick_x * 2 : 0;

        // Packing and sending messages
        RS485::getInstance().txMode(); // Ensure in tx mode

        uint8_t packed_msg_up = Protocol::pack(original_control_msg_up);
        RS485::getInstance().send(packed_msg_up);
        
        uint8_t packed_msg_down = Protocol::pack(original_control_msg_down);
        RS485::getInstance().send(packed_msg_down);

        uint8_t packed_msg_left = Protocol::pack(original_control_msg_left);
        RS485::getInstance().send(packed_msg_left);

        uint8_t packed_msg_right = Protocol::pack(original_control_msg_right);
        RS485::getInstance().send(packed_msg_right);

        // What robot receives (unpacking)
        ControlMessage received_control_msg_up;
        Protocol::unpack(packed_msg_up, received_control_msg_up);

        ControlMessage received_control_msg_down;
        Protocol::unpack(packed_msg_down, received_control_msg_down);

        ControlMessage received_control_msg_left;
        Protocol::unpack(packed_msg_left, received_control_msg_left);

        ControlMessage received_control_msg_right;
        Protocol::unpack(packed_msg_right, received_control_msg_right);

        MotorInformation motor_info_up;
        motor_info_up.motor_id = 0;
        motor_info_up.thrust = received_control_msg_up.thrust;
        UI::getInstance().update(motor_info_up);

        MotorInformation motor_info_down;
        motor_info_down.motor_id = 1;
        motor_info_down.thrust = received_control_msg_down.thrust;
        UI::getInstance().update(motor_info_down);

        MotorInformation motor_info_left;
        motor_info_left.motor_id = 2;
        motor_info_left.thrust = received_control_msg_left.thrust;
        UI::getInstance().update(motor_info_left);

        MotorInformation motor_info_right;
        motor_info_right.motor_id = 3;
        motor_info_right.thrust = received_control_msg_right.thrust;
        UI::getInstance().update(motor_info_right);

        BatteryInformation battery_info;
        battery_info.type = BatteryType::DUALSHOCK;
        battery_info.percentage = PS4.Battery() * 10; // PS4.Battery() returns 0-10
        battery_info.charging = PS4.Charging();
        UI::getInstance().update(battery_info);

        BatteryInformation controller_battery_info;
        controller_battery_info.type = BatteryType::CONTROLLER;
        controller_battery_info.percentage = 75; // Dummy value
        UI::getInstance().update(controller_battery_info);


        if (iterations >= 100)
        {
          // 10 seconds elapsed
          Serial.println("[INFO] Requesting battery...");

          BatteryInformation rov_battery_info;
          rov_battery_info.type = BatteryType::ROV;

          BatteryRequestMessage request;
          uint8_t packed_request = Protocol::pack(request);
          RS485::getInstance().send(packed_request);
          RS485::getInstance().wait(); // Ensure data is sent
          RS485::getInstance().rxMode(); // Switch to rx mode to receive response
          RS485::getInstance().wait(); // Give time to switch and receive
          RS485::getInstance().wait(); // Give time to switch and receive

          int received_response = RS485::getInstance().readLast(); // Receive last byte

          // Go back to tx mode
          RS485::getInstance().txMode();
          RS485::getInstance().wait(); // Wait the counter part to switch

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
            }
          }
          UI::getInstance().update(rov_battery_info);

          iterations = 0;
        }
    }

    UI::getInstance().refresh();

    delay(100);
}

*/

#endif