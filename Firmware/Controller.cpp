#include "Controller.hpp"

Controller::Controller()
{

}

void Controller::init()
{
    Serial.begin(115200);
    
    pinMode(PIN_DE_RE, OUTPUT);
    digitalWrite(PIN_DE_RE, HIGH); // MAX-458 TX MODE

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

        // Left joystick controls UP/DOWN
        int left_stick_y = analogRead(PIN_JOYSTICK_LEFT_Y);

        uint8_t up_thrust = map(left_stick_y, JOYSTICK_CENTER + JOYSTICK_DEADZONE, ADC_MAX_VALUE, 0, 255);
        uint8_t down_thrust = map(left_stick_y, 0, JOYSTICK_CENTER - JOYSTICK_DEADZONE, 0, 255);
        
        // Right joystick controls FORWARD/BACKWARD and LEFT/RIGHT (Differential drive)
        int right_stick_x = analogRead(PIN_JOYSTICK_RIGHT_X);
        int right_stick_y = analogRead(PIN_JOYSTICK_RIGHT_Y);

        uint8_t left_thust = 0;
        uint8_t right_thust = 0;
        joystickToDifferentialDrive(right_stick_x, right_stick_y, left_thust, right_thust);

        // Send command
        Command command;
        command.code = CMD_CONTROL;
        command.param1 = up_thrust;
        command.param2 = down_thrust;
        command.param3 = left_thust;
        command.param4 = right_thust;

        Serial.write(SYNC_BYTE);
        Serial.write((uint8_t*)&command, sizeof(Command));
        Serial.flush(); // Ensure all previous data is sent

        // Release RS485 mutex
        xSemaphoreGive(rs485_mutex);

        vTaskDelayUntil(&last_wake_time, period);
        
        digitalWrite(5, on ? HIGH : LOW);
        on = !on;
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
        Command command;
        command.code = CMD_REQUEST_BATTERY;
        command.param1 = 0;
        command.param2 = 0;
        command.param3 = 0;
        command.param4 = 0;

        Serial.write(SYNC_BYTE);
        Serial.write((uint8_t*)&command, sizeof(Command));
        Serial.flush(); // Ensure all previous data is sent

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