#pragma once

#include "Arduino.h"

#define PIN_RED    8
#define PIN_GREEN  9
#define PIN_BLUE   10

class StatusLed
{
    // Singleton pattern
public:
    static StatusLed& getInstance()
    {
        static StatusLed instance;
        return instance;
    }

    void begin();

    bool isOn() const { return on; }

    void setRGB(uint8_t r, uint8_t g, uint8_t b);

    void red();
    void green();
    void blue();
    void yellow();
    void purple();
    void cyan();
    void white();
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