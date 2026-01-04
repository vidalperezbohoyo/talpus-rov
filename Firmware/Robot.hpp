#pragma once

#include "Arduino.h"
#include "Defines.hpp"

class Robot
{
public:
    Robot();

    void init();

    void loop();
private:
    
    const int PIN_MOTOR_1 = 3;
    const int PIN_MOTOR_2 = 4;
    const int PIN_MOTOR_3 = 5;
    const int PIN_MOTOR_4 = 6;

    const int PIN_LIGHTS = 7;
};
