#include "Robot.hpp"
#include "OTA.hpp"

#define PIN_OTA 6

Robot robot;


void setup()
{
    pinMode(PIN_OTA, INPUT_PULLUP);

    // Check if OTA mode
    if (digitalRead(PIN_OTA) == LOW)
    {
        pinMode(PIN_LED_B, OUTPUT);
        digitalWrite(PIN_LED_B, HIGH);
        OTA::update(); // Will not return
    }

    robot.init();
}

void loop()
{
    robot.loop();
}