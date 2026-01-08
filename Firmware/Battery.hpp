#pragma once

#include "Arduino.h"
#include "Defines.hpp"

class Battery
{
public:
    // Singleton instance
    static Battery& getInstance()
    {
        static Battery instance;
        return instance;
    }

    void init();

    float getVoltage(); // Returns voltage in volts

    uint8_t getPercentage(); // Returns percentage [0-100]

};