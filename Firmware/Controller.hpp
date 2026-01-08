#pragma once

#include "Arduino.h"
#include "Defines.hpp"

#define PIN_JOYSTICK_LEFT_X  0
#define PIN_JOYSTICK_LEFT_Y  1
#define PIN_JOYSTICK_RIGHT_X 2
#define PIN_JOYSTICK_RIGHT_Y 3

#define JOYSTICK_DEADZONE 50 // In ADC units
#define JOYSTICK_CENTER (ADC_MAX_VALUE / 2)

class Controller
{
public:
    Controller();

    void init();

    void loop();

private:

    static void task_wrapper_readControlInputs(void* params);
    void task_readControlInputs();

    static void task_wrapper_requestBattery(void* params);
    void task_requestBattery();

    void joystickToDifferentialDrive(int joy_x, int joy_y, uint8_t& left_thust, uint8_t& right_thust);

    SemaphoreHandle_t rs485_mutex;
};