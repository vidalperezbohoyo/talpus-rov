#pragma once

#include "Arduino.h"
#include <PS4Controller.h>
#include "RS485.hpp"
#include "Defines.hpp"

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