#if defined(ARDUINO_ESP32C3_DEV) // Only for ESP32-C3

#include "StatusLed.hpp"

void StatusLed::init()
{
    pinMode(PIN_GREEN, OUTPUT);
    pinMode(PIN_BLUE, OUTPUT);
    off();
}

void StatusLed::green()
{
    digitalWrite(PIN_GREEN, HIGH);
    digitalWrite(PIN_BLUE, LOW);
}

void StatusLed::blue()
{
    digitalWrite(PIN_GREEN, LOW);
    digitalWrite(PIN_BLUE, HIGH);
}

void StatusLed::off()
{
    digitalWrite(PIN_GREEN, LOW);
    digitalWrite(PIN_BLUE, LOW);
}

#endif

