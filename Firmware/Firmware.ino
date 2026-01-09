#include "StatusLed.hpp"

#define ROBOT_FW

#if defined(ROBOT_FW)
    #include "Robot.hpp"
    #include "OTA.hpp"
    Robot robot;
#else
    #include "Controller.hpp"
    Controller controller;
#endif

void setup()
{  
    #if defined(ROBOT_FW)
        OTA::init();
        StatusLed::getInstance().init();

        // Check if OTA mode
        if (OTA::requestUpdate())
        {
            StatusLed::getInstance().blue();
            OTA::update(); // Will not return
        }
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