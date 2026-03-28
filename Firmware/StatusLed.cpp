#if defined(ARDUINO_ESP32C3_DEV) // Only for ESP32-C3

#include "StatusLed.hpp"

void StatusLed::init()
{
    pinMode(PIN_STATUS_LED, OUTPUT);
    off();
}

void StatusLed::on()
{
    digitalWrite(PIN_STATUS_LED, HIGH);
    on_ = true;
}

void StatusLed::off()
{
    digitalWrite(PIN_STATUS_LED, LOW);
    on_ = false;
}

#endif

