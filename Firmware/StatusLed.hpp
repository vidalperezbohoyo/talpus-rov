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

    bool isOn() const { return on_; }

    void on();
    void off();
    void toggle() { if (on_) off(); else on(); }
   
private:
    StatusLed() = default;
    ~StatusLed() = default;
    StatusLed(const StatusLed&) = delete;
    StatusLed& operator=(const StatusLed&) = delete;
    StatusLed(StatusLed&&) = delete;
    StatusLed& operator=(StatusLed&&) = delete;

    bool on_ = false;
};
#endif