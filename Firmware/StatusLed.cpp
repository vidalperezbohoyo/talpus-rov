#if defined(ARDUINO_ESP32C3_DEV) // Only for ESP32-C3

#include "StatusLed.hpp"

void StatusLed::init()
{
    pinMode(PIN_RED, OUTPUT);
    pinMode(PIN_GREEN, OUTPUT);
    pinMode(PIN_BLUE, OUTPUT);
    off();
}

void StatusLed::setRGB(uint8_t r, uint8_t g, uint8_t b)
{
    analogWrite(PIN_RED, r);
    analogWrite(PIN_GREEN, g);
    analogWrite(PIN_BLUE, b);

    on = (r > 0) || (g > 0) || (b > 0);
}

void StatusLed::red()
{
    setRGB(255, 0, 0);
}

void StatusLed::green()
{
    setRGB(0, 255, 0);
}

void StatusLed::blue()
{
    setRGB(0, 0, 255);
}

void StatusLed::yellow()
{
    setRGB(255, 255, 0);
}

void StatusLed::purple()
{
    setRGB(128, 0, 128);
}

void StatusLed::cyan()
{
    setRGB(0, 255, 255);
}

void StatusLed::white()
{
    setRGB(255, 255, 255);
}

void StatusLed::off()
{
    setRGB(0, 0, 0);
}

#endif

