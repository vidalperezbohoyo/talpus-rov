#if defined(ARDUINO_ESP32C3_DEV) // Only for ESP32-C3

#include "Robot.hpp"

Robot::Robot()
{

}

void Robot::init()
{
    StatusLed::getInstance().green(); // Indicate ready

    RS485::getInstance().init(ROBOT_PIN_RX, ROBOT_PIN_TX, ROBOT_PIN_DE_RE);

    // Initialize motors pins
    pinMode(PIN_MOTOR_1, OUTPUT);
    pinMode(PIN_MOTOR_2, OUTPUT);
    pinMode(PIN_MOTOR_3, OUTPUT);
    pinMode(PIN_MOTOR_4, OUTPUT);

    // Set motors to zero thrust
    analogWrite(PIN_MOTOR_1, 0);
    analogWrite(PIN_MOTOR_2, 0);
    analogWrite(PIN_MOTOR_3, 0);
    analogWrite(PIN_MOTOR_4, 0);

    // Lights pin
    pinMode(PIN_LIGHTS, OUTPUT);
    analogWrite(PIN_LIGHTS, 0); // Lights off

    // Watchdog task
    /*
    xTaskCreate(
        task_wrapper_watchdog,
        "WatchdogTask",
        4096, // Stack       
        this,
        1,
        NULL
    );
    */
}

void Robot::loop()
{
    // Wait for commands
    while (true)
    {
        if (RS485::getInstance().available())
        {
            int read = RS485::getInstance().read();
            if (read < 0)
            {
                continue;
            }

            uint8_t byte = static_cast<uint8_t>(read);

            if (Protocol::getMessageType(byte) == MessageType::CONTROL)
            {
                ControlMessage msg;
                
                // Unpack command
                Protocol::unpack(byte, msg);

                // Process command
                switch (msg.motor_id)
                {
                    case 0: // Motor 1
                        //analogWrite(PIN_MOTOR_1, msg.thrust);
                        StatusLed::getInstance().setRGB(0, 0, msg.thrust); // Indicate activity
                        break;
                    case 1: // Motor 2
                        //analogWrite(PIN_MOTOR_2, msg.thrust);
                        break;
                    case 2: // Motor 3
                        //analogWrite(PIN_MOTOR_3, msg.thrust);
                        break;
                    case 3: // Motor 4
                        //analogWrite(PIN_MOTOR_4, msg.thrust);
                        break;
                    default:
                        break;
                }

                // Update last command time
                last_command_time = millis();
            }
            else
            {
              StatusLed::getInstance().red();
            }
            
        }
    }
}

void Robot::task_wrapper_watchdog(void* params)
{
    Robot* robot = static_cast<Robot*>(params);
    robot->task_watchdog();
}

void Robot::task_watchdog()
{
    const TickType_t period = pdMS_TO_TICKS(1000); // 1 second
    TickType_t last_wake_time = xTaskGetTickCount();

    while (true)
    {
        // Check if there is lot of time without serial data
        // If so, go up fast to surface
        if (millis() - last_command_time > EMERGENCY_RTL_TIMEOUT_MS)
        {
            StatusLed::getInstance().red();
            analogWrite(PIN_MOTOR_1, 255); // Full up
            analogWrite(PIN_MOTOR_2, 0);
            analogWrite(PIN_MOTOR_3, 0);
            analogWrite(PIN_MOTOR_4, 0);
        }
    
        vTaskDelayUntil(&last_wake_time, period);
    }
}

#endif