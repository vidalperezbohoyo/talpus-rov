#pragma once
#include "Arduino.h"
#include "Defines.hpp"

class Battery
{
public:
    static Battery& getInstance()
    {
        static Battery instance;
        return instance;
    }

    void init();

#if defined(ARDUINO_ESP32_DEV) // Only for ESP32 
    BatteryInformation info();
#endif

private:
    Battery() = default;
    ~Battery() = default;
    Battery(const Battery&) = delete;
    Battery& operator=(const Battery&) = delete;

    // IIR filter for ADC readings
    uint16_t readADC();

};
