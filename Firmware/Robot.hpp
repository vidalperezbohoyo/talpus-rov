#pragma once

#include "Arduino.h"
#include "RS485.hpp"
#include "Defines.hpp"
#include "StatusLed.hpp"

class Robot
{
public:
    Robot();

    void init();

    void loop();

private:

    static void task_wrapper_watchdog(void* params);
    void task_watchdog();

    unsigned long last_command_time = 0;
};
