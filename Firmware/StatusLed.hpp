#pragma once
#if defined(ARDUINO_ESP32C3_DEV) // Only for ESP32-C3

#include "Arduino.h"
#include "Defines.hpp"

class StatusLed
{
    // Singleton pattern
public:
    static StatusLed& getInstance()
    {
        static StatusLed instance;
        return instance;
    }

    void init();

    bool isOn() const { return on; }

    void green();
    void blue();

    void off();
   
private:
    StatusLed() = default;
    ~StatusLed() = default;
    StatusLed(const StatusLed&) = delete;
    StatusLed& operator=(const StatusLed&) = delete;
    StatusLed(StatusLed&&) = delete;
    StatusLed& operator=(StatusLed&&) = delete;

    bool on = false;
};
#endif