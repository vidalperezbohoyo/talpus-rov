
/*#include "Controller.hpp"

Controller::Controller()
{

}

void Controller::init()
{
    RS485::getInstance().init(CONTROLLER_PIN_RX, CONTROLLER_PIN_TX, CONTROLLER_PIN_DE_RE);
    PS4.begin("e0:d4:e8:72:14:37");

    while (!PS4.isConnected());

    pinMode(5, OUTPUT);

    digitalWrite(5, HIGH); 
    delay(1000);
    digitalWrite(5, LOW);
    delay(1000);


    rs485_mutex = xSemaphoreCreateMutex();

    xTaskCreate(
        task_wrapper_readControlInputs,
        "ReadControlInputsTask",
        6144, // Stack       
        this,
        1,
        NULL
    );
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

void Controller::joystickToDifferentialDrive(int joy_x, int joy_y, uint8_t& left_thust, uint8_t& right_thust)
{
    // 1. Deadzone
    if (std::abs(joy_x - JOYSTICK_CENTER) < JOYSTICK_DEADZONE)
    {
        joy_x = JOYSTICK_CENTER;
    }

    if (std::abs(joy_y - JOYSTICK_CENTER) < JOYSTICK_DEADZONE)
    {
        joy_y = JOYSTICK_CENTER;
    }

    // 2. Normalize [-1.0, 1.0]
    float x = (float(joy_x) - JOYSTICK_CENTER) / JOYSTICK_CENTER;
    float y = (float(joy_y) - JOYSTICK_CENTER) / JOYSTICK_CENTER;

    // Clamp for safety
    x = std::clamp(x, -1.0f, 1.0f);
    y = std::clamp(y, -1.0f, 1.0f);

    // 3. Differential mixing
    float v_left  = y + x;
    float v_right = y - x;

    // 4. Normalization (avoids saturation)
    float max_val = std::max({ std::abs(v_left), std::abs(v_right), 1.0f });
    v_left  /= max_val;
    v_right /= max_val;

    // Return values as uint8_t thrust commands
    left_thust = (v_left  > 0.0f) ? uint8_t(v_left  * 255) : 0;
    right_thust = (v_right > 0.0f) ? uint8_t(v_right * 255) : 0;
}
    */