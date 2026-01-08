#pragma once

#include "Arduino.h"
#include "Defines.hpp"
#include "StatusLed.hpp"

#define PIN_MOTOR_1 3 // Up
#define PIN_MOTOR_2 5 // Down
#define PIN_MOTOR_3 6 // Left
#define PIN_MOTOR_4 9 // Right

//#define PIN_LIGHTS 10

class Robot
{
public:
    Robot();

    void init();

    void loop();

private:

};
