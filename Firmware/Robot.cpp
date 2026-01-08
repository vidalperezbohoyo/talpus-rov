#include "Robot.hpp"

Robot::Robot()
{

}

void Robot::init()
{
    Serial.begin(115200);
    
    pinMode(PIN_DE_RE, OUTPUT);
    digitalWrite(PIN_DE_RE, LOW); // MAX-458 RX MODE

    StatusLed::getInstance().green(); // Indicate ready
}

void Robot::loop()
{
    // Wait for commands
    Command cmd;

    while (true)
    {
        if (Serial.available())
        {
            StatusLed::getInstance().yellow();
            int sync_byte = Serial.read();
            if ((sync_byte != -1) && (sync_byte == SYNC_BYTE))
            {
                // Read command struct

                cmd = {0}; // Clear
                Serial.readBytes((uint8_t*)&cmd, sizeof(Command));

                switch (cmd.code)
                {
                    case (CMD_CONTROL):
                    {
                        // analogWrite(PIN_MOTOR_1, cmd.param1);
                        // analogWrite(PIN_MOTOR_2, cmd.param2);
                        // analogWrite(PIN_MOTOR_3, cmd.param3);
                        // analogWrite(PIN_MOTOR_4, cmd.param4);
                        StatusLed::getInstance().setRGB(cmd.param1, cmd.param2, 0);
                        break;
                    }
                    // case (CMD_LIGHTS):
                    // {
                    //     analogWrite(PIN_LIGHTS, cmd.param1);
                    // }
                    // case (CMD_BATTERY):
                    // {
                    //     // Read

                    //     // Send response
                    //     Response res;
                    //     res.code = RES_OK;
                    //     res.value = 124;

                    //     // MAX-458 TX MODE
                    //     // MAX-458 RX MODE
                    // }
                    default:
                    {
                        // Unknown CMD
                        break;
                    }
                }
            }
        }
    }
}