#pragma once
#if defined(ARDUINO_ESP32C3_DEV) // Only for ESP32-C3

#include "Arduino.h"

#include <WiFi.h>
#include <WebServer.h>
#include <ElegantOTA.h>

#include "Defines.hpp"

class OTA
{
public:
    static void init();

    static bool requestUpdate();

    static void update();
};

#endif