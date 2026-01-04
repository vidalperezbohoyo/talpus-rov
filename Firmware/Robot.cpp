#include "Robot.hpp"

Robot::Robot()
{

}

void Robot::loop()
{
    // Wait for commands
    Command cmd;

    while (true)
    {
        if (Serial.available())
        {
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
                        analogWrite(PIN_MOTOR_1, cmd.param1);
                        analogWrite(PIN_MOTOR_2, cmd.param2);
                        analogWrite(PIN_MOTOR_3, cmd.param3);
                        analogWrite(PIN_MOTOR_4, cmd.param4);
                        break;
                    }
                    case (CMD_LIGHTS):
                    {
                        analogWrite(PIN_LIGHTS, cmd.param1);
                    }
                    case (CMD_BATTERY):
                    {
                        // Read

                        // Send response
                        Response res;
                        res.code = RES_OK;
                        res.value = 124;

                        // MAX-458 TX MODE
                        // MAX-458 RX MODE
                    }
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