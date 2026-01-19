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


    SemaphoreHandle_t rs485_mutex;
};

#endif