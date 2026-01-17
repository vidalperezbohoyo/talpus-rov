#pragma once

#if defined(ARDUINO_ESP32_DEV) // Only for ESP32

#include "Arduino.h"
#include "lvgl.h"
//#include "Setup42_ILI9341_ESP32.h"
#include "TFT_eSPI.h"
#include "Defines.hpp"

class UI
{
private:
    /* data */
public:
    // Singleton instance
    static UI& getInstance()
    {
        static UI instance;
        return instance;
    }

    void init();

    void update(const MotorInformation& motor_info);

    void update(const BatteryInformation& battery_info);

    static void refreshScreen(void* params);

private:
    UI(/* args */);
    ~UI();
    UI(const UI&) = delete;
    UI& operator=(const UI&) = delete;
    UI(UI&&) = delete;
    UI& operator=(UI&&) = delete;

    void createDashboard();
    void createBatteriesContainer();
    void createMotorsThrustContainer();

    // Multi-task sync
    QueueHandle_t motor_info_queue_;
    QueueHandle_t battery_info_queue_;

    // Rewritable UI elements
    lv_obj_t* rov_battery_percentage_label_;
    lv_obj_t* rov_battery_voltage_label_;

    lv_obj_t* controller_battery_percentage_label_;
    lv_obj_t* controller_battery_voltage_label_;

    lv_obj_t* dualshock_battery_label_;
    lv_obj_t* dualshock_charging_label_;

    lv_obj_t* motor_up_thust_bar_;
    lv_obj_t* motor_down_thust_bar_;
    lv_obj_t* motor_left_thust_bar_;
    lv_obj_t* motor_right_thust_bar_;

    lv_obj_t* motor_up_thust_label_;
    lv_obj_t* motor_down_thust_label_;
    lv_obj_t* motor_left_thust_label_;
    lv_obj_t* motor_right_thust_label_;

    // LVGL display buffer
    uint8_t* draw_buf_;

};

#endif
