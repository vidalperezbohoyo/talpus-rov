#pragma once

#if defined(ARDUINO_ESP32_DEV) // Only for ESP32

#include "Arduino.h"
#include <PS4Controller.h>
#include "RS485.hpp"
#include "Defines.hpp"
#include "UI.hpp"
#include "Battery.hpp"

class Controller
{
public:
    Controller();

    void init();

    void loop();

    static void rovControlTask(void* params);
    static void rovBatteryTask(void* params);
    static void controllerBatteryTask(void* params);

private:
    // Queues to send Structs between tasks
    QueueHandle_t battery_response_queue_;
    QueueHandle_t lights_message_queue_;

    // Mutex for communication access
    SemaphoreHandle_t comms_mutex_;

    // Other variables
    uint8_t lights_intensity_ = 0;
};

#endif