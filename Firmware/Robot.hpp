#pragma once

#if defined(ARDUINO_ESP32C3_DEV) // Only for ESP32-C3

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

    void processControlMessage(const ControlMessage& msg);
    void processBatteryRequestMessage();

    static void task_wrapper_watchdog(void* params);
    void task_watchdog();

    unsigned long last_command_time = 0;
};

#endif