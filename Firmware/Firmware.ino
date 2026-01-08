#include "Robot.hpp"
#include "Controller.hpp"
#include "OTA.hpp"
#include "StatusLed.hpp"

#define PIN_OTA 6

#define ROBOT_FW

#if defined(ROBOT_FW)
    Robot robot;
#else
    Controller controller;
#endif

void setup()
{
    pinMode(PIN_OTA, INPUT_PULLUP);

    StatusLed::getInstance().begin();

    // Check if OTA mode
    if (digitalRead(PIN_OTA) == LOW)
    {
        StatusLed::getInstance().blue();
        OTA::update(); // Will not return
    }

    #if defined(ROBOT_FW)
        robot.init();
    #else
        controller.init();
    #endif
}

void loop()
{
    #if defined(ROBOT_FW)
        robot.loop();
    #else
        controller.loop();
    #endif
}