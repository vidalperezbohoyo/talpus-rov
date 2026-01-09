#pragma once

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