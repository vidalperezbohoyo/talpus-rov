#pragma once

#include "Arduino.h"

#include <WiFi.h>
#include <WebServer.h>
#include <ElegantOTA.h>

class OTA
{
public:
    static void update();
};