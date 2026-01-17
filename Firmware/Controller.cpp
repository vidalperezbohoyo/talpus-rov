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

    Serial.println("[Controller::init] Waiting for PS4 Controller connection...");
    while (!PS4.isConnected());
    Serial.println("[Controller::init] PS4 Controller connected");

    rs485_mutex = xSemaphoreCreateMutex();

    xTaskCreate(
        task_wrapper_readControlInputs,
        "ReadControlInputsTask",
        6144, // Stack       
        this,
        1,
        NULL
    );

    xTaskCreate(
        UI::refreshScreen,
        "UITask",
        8000, // Stack       
        this,
        1,
        NULL
    );

  Serial.println("[Controller::init] Init done");

}

void Controller::loop()
{
    vTaskDelay(portMAX_DELAY); // No CPU work in main loop
}

void Controller::task_wrapper_readControlInputs(void* params)
{
    Controller* controller = static_cast<Controller*>(params);
    controller->task_readControlInputs();
}

void Controller::task_readControlInputs()
{
    const TickType_t period = pdMS_TO_TICKS(100); // 100 ms -> 10 Hz
    TickType_t last_wake_time = xTaskGetTickCount();

    bool on = true;

    while (true)
    {
        // Take RS485 mutex
        xSemaphoreTake(rs485_mutex, portMAX_DELAY);
        
        int8_t left_stick_x = PS4.LStickX();
        int8_t left_stick_y = PS4.LStickY();
        int8_t right_stick_x = PS4.RStickX();
        int8_t right_stick_y = PS4.RStickY();

        uint8_t l2_value = PS4.L2Value();
        uint8_t r2_value = PS4.R2Value();

        uint8_t left_thrust = 0;

        ControlMessage msg;
        msg.motor_id = 0; // Motor 1
        msg.thrust = r2_value;
        analogWrite(5, msg.thrust);
        RS485::getInstance().send(Protocol::pack(msg));

        msg.motor_id = 1; // Motor 2
        msg.thrust = l2_value;
        RS485::getInstance().send(Protocol::pack(msg));

        
        // Release RS485 mutex
        xSemaphoreGive(rs485_mutex);

        // Show information
        MotorInformation motor_info;
        motor_info.motor_id = 0;
        motor_info.thrust = r2_value;
        UI::getInstance().update(motor_info);

        vTaskDelayUntil(&last_wake_time, period);
    }
}

void Controller::task_wrapper_requestBattery(void* params)
{
    Controller* controller = static_cast<Controller*>(params);
    controller->task_requestBattery();
}

void Controller::task_requestBattery()
{
    const TickType_t period = pdMS_TO_TICKS(5000); // 5 seconds
    TickType_t last_wake_time = xTaskGetTickCount();

    while (true)
    {
        // Take RS485 mutex
        xSemaphoreTake(rs485_mutex, portMAX_DELAY);

        // Send battery request command


        // Release RS485 mutex
        xSemaphoreGive(rs485_mutex);

        vTaskDelayUntil(&last_wake_time, period);
    }
}

#endif