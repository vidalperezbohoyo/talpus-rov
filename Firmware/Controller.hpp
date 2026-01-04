#pragma once

#include "Arduino.h"
#include <ArduinoThread.h>
#include "Defines.hpp"

class Controller
{
public:
    Controller();

    void init();

    void loop();
private:


    const int PIN_JOYSTICK_1 = 3;
    const int PIN_JOYSTICK_1 = 4;
    const int PIN_JOYSTICK_1 = 5;
    const int PIN_JOYSTICK_1 = 6;

    const int PIN_LIGHTS_ON_OFF = 7;
};
