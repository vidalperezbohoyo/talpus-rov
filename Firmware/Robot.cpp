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

    Serial.begin(115200);
}

void Robot::loop()
{
    // Wait for commands
    RS485::getInstance().rxMode(); // Ensure in rx mode

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
                if (Protocol::unpack(byte, msg))
                {
                    processControlMessage(msg);
                }

                //Serial.print("Received CONTROL with id: "); Serial.print(static_cast<int>(msg.motor_id)); Serial.print(" and value: "); Serial.println(static_cast<int>(msg.thrust));

                // Update last command time
                last_command_time = millis();
            }
            else if (Protocol::getMessageType(byte) == MessageType::REQUEST_BATTERY)
            {
                Serial.println("Received REQUEST_BATTERY");
                RS485::getInstance().txMode(); // Switch to tx mode to send response
                RS485::getInstance().wait(); // Wait before responding              
                processBatteryRequestMessage();
                RS485::getInstance().halfWait();
                RS485::getInstance().rxMode(); // Switch back to rx mode
            }
            else
            {
              StatusLed::getInstance().red();
            }
            
        }
    }
}

void Robot::processControlMessage(const ControlMessage& msg)
{
    switch (msg.motor_id)
    {
        case 0: // Motor 1
            StatusLed::getInstance().setRGB(0, 0, msg.thrust); // Indicate activity

            //analogWrite(PIN_MOTOR_1, msg.thrust);
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
}

void Robot::processBatteryRequestMessage()
{
    // Prepare response message
    BatteryResponseMessage response;
    response.percentage = 77;

    uint8_t packed_response = Protocol::pack(response);

    // Send response
    RS485::getInstance().send(packed_response);
    // RS485::getInstance().send(packed_response);
    // RS485::getInstance().send(packed_response);
}

#endif