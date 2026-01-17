#pragma once

#if defined(ARDUINO_ESP32_DEV) // Only for ESP32

#include "Arduino.h"
#include <PS4Controller.h>
#include "RS485.hpp"
#include "Defines.hpp"
#include "UI.hpp"

class Controller
{
public:
    Controller();

    void init();

    void loop();

private:

    static void task_wrapper_readControlInputs(void* params);
    void task_readControlInputs();

    static void task_wrapper_requestBattery(void* params);
    void task_requestBattery();

    SemaphoreHandle_t rs485_mutex;
};

#endif