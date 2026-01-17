
#if defined(ARDUINO_ESP32C3_DEV) // Only for ESP32-C3
    #include "StatusLed.hpp"
    #include "Robot.hpp"
    #include "OTA.hpp"
    Robot robot;
#elif defined(ARDUINO_ESP32_DEV)
    #include "Controller.hpp"
    Controller controller;
#else
    #error "Unsupported platform"
#endif

void setup()
{  
    #if defined(ARDUINO_ESP32C3_DEV)
        OTA::init();
        StatusLed::getInstance().init();

        // Check if OTA mode
        if (OTA::requestUpdate())
        {
            StatusLed::getInstance().blue();
            OTA::update(); // Will not return
        }
        robot.init();
    #elif defined(ARDUINO_ESP32_DEV)
        controller.init();
    #endif
}

void loop()
{
    #if defined(ARDUINO_ESP32C3_DEV)
        robot.loop();
    #elif defined(ARDUINO_ESP32_DEV)
        controller.loop();
    #endif
}