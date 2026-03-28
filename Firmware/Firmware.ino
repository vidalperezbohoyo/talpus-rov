
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
            // Blink LED fast
            for (int i = 0; i < 10; i++)
            {
                StatusLed::getInstance().on();
                delay(100);
                StatusLed::getInstance().off();
                delay(100);
            }
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