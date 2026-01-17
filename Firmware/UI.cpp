#define ARDUINO_ESP32_DEV // Only for ESP32
#if defined(ARDUINO_ESP32_DEV) // Only for ESP32

#include "UI.hpp"

UI::UI(/* args */)
{
}

UI::~UI()
{
}

void UI::init()
{
    lv_init();

    draw_buf_ = new uint8_t[DRAW_BUF_SIZE];

    lv_display_t * disp;
    disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf_, DRAW_BUF_SIZE);

    // Create queues
    motor_info_queue_ = xQueueCreate(10, sizeof(MotorInformation));
    battery_info_queue_ = xQueueCreate(10, sizeof(BatteryInformation));
    
    createDashboard();

}

void UI::update(const MotorInformation& motor_info)
{
    if (motor_info_queue_ == nullptr)
        return;
    Serial.println("UI::update - Sending motor info to queue");
    xQueueSend(motor_info_queue_, &motor_info, 0);
}

void UI::update(const BatteryInformation& battery_info)
{
    if (battery_info_queue_ == nullptr)
        return;

    xQueueSend(battery_info_queue_, &battery_info, 0);
}

void UI::refreshScreen(void* params)
{
    UI* ui = static_cast<UI*>(params);

    const TickType_t period = pdMS_TO_TICKS(100); // 100 ms -> 10 Hz
    TickType_t last_wake_time = xTaskGetTickCount();

    while (true)
    {
        // Process queues
        if (ui->motor_info_queue_ == NULL || ui->battery_info_queue_ == NULL)
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        MotorInformation motor_update;
        BatteryInformation battery_update;

        while (xQueueReceive(ui->motor_info_queue_, &motor_update, 0) == pdTRUE)
        {
            switch (motor_update.motor_id)
            {
            case 0: // UP
                lv_bar_set_value(ui->motor_up_thust_bar_, motor_update.thrust, LV_ANIM_OFF);
                lv_label_set_text_fmt(ui->motor_up_thust_label_, "%d", motor_update.thrust);
                break;
            case 1: // DOWN
                lv_bar_set_value(ui->motor_down_thust_bar_, motor_update.thrust, LV_ANIM_OFF);
                lv_label_set_text_fmt(ui->motor_down_thust_label_, "%d", motor_update.thrust);
                break;
            case 2: // LEFT
                lv_bar_set_value(ui->motor_left_thust_bar_, motor_update.thrust, LV_ANIM_OFF);
                lv_label_set_text_fmt(ui->motor_left_thust_label_, "%d", motor_update.thrust);
                break;
            case 3: // RIGHT
                lv_bar_set_value(ui->motor_right_thust_bar_, motor_update.thrust, LV_ANIM_OFF);
                lv_label_set_text_fmt(ui->motor_right_thust_label_, "%d", motor_update.thrust);
                break;
            }
        }

        while (xQueueReceive(ui->battery_info_queue_, &battery_update, 0) == pdTRUE)
        {
            switch (battery_update.type)
            {
            case BatteryType::ROV:
                lv_label_set_text_fmt(ui->rov_battery_percentage_label_, "%d%%", battery_update.percentage);
                lv_label_set_text_fmt(ui->rov_battery_voltage_label_, "%.2fV", battery_update.voltage);
                break;
            case BatteryType::CONTROLLER:
                lv_label_set_text_fmt(ui->controller_battery_percentage_label_, "%d%%", battery_update.percentage);
                lv_label_set_text_fmt(ui->controller_battery_voltage_label_, "%.2fV", battery_update.voltage);
                break;
            case BatteryType::DUALSHOCK: 
                lv_label_set_text_fmt(ui->dualshock_battery_label_, "%d%%", battery_update.percentage);
                
                if (battery_update.charging)
                    lv_label_set_text(ui->dualshock_charging_label_, "Charging");
                else
                    lv_label_set_text(ui->dualshock_charging_label_, "Not charging");
                break;
            }
        }

        lv_tick_inc(100);
        lv_timer_handler(); /* let the GUI do its work */
        vTaskDelayUntil(&last_wake_time, period);
        Serial.println("Refreshing screen");
    }
}

void UI::createDashboard()
{
    createBatteriesContainer();
    createMotorsThrustContainer();
}

void UI::createBatteriesContainer()
{
    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, 320, 135);
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 0);


    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont,
                            LV_FLEX_ALIGN_SPACE_EVENLY,
                            LV_FLEX_ALIGN_CENTER,
                            LV_FLEX_ALIGN_CENTER);


    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    for (int i = 0; i < 3; i++)
    {
        // Main container
        lv_obj_t * cell = lv_obj_create(cont);
        lv_obj_set_size(cell, 90, 140);
        lv_obj_set_style_bg_opa(cell, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(cell, 0, 0);
        lv_obj_clear_flag(cell, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_flex_flow(cell, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(cell,
                            LV_FLEX_ALIGN_CENTER,
                            LV_FLEX_ALIGN_CENTER,
                            LV_FLEX_ALIGN_CENTER);

        // Title
        lv_obj_t * title = lv_label_create(cell);
        lv_obj_set_style_text_font(title, &lv_font_montserrat_12, 0);

        // Arc
        lv_obj_t * arc = lv_arc_create(cell);
        lv_obj_set_size(arc, 90, 90);
        lv_arc_set_range(arc, 0, 100);
        lv_arc_set_value(arc, 0); // 0 as default
        lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
        lv_arc_set_bg_angles(arc, 90 + 45, 90 - 45);
        lv_arc_set_rotation(arc, 0);

        switch (i)
        {
            case 0:
            {
                lv_label_set_text(title, "ROV");
                break;
            }
            case 1:
            {
                lv_label_set_text(title, "GroundBox");
                break;
            }
            case 2:
            {
                lv_label_set_text(title, "DualShock4");
                break;
            }
        }

        /* Texto central */
        // lv_obj_t * label = lv_label_create(arc);
        // lv_label_set_text_fmt(label, "%d%%\n%.2fV", percent, voltage);
        // lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        // lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
        // lv_obj_center(label);

        /*
        TODO
            if(percent > 60)
                lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
            else if(percent > 30)
                lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_INDICATOR);
            else
                lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);
        */

    }
}

void UI::createMotorsThrustContainer()
{
    lv_obj_t * cont = lv_obj_create(lv_scr_act());

    lv_obj_set_width(cont, 320);
    lv_obj_set_height(cont, LV_SIZE_CONTENT);

    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 140);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont,
                        LV_FLEX_ALIGN_START,  
                        LV_FLEX_ALIGN_START,  
                        LV_FLEX_ALIGN_CENTER);

    lv_obj_set_style_pad_all(cont, 0, 0);    
    lv_obj_set_style_pad_row(cont, 0, 0);     
    lv_obj_set_style_pad_column(cont, 0, 0);   

    lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, -5);

    
    for (int i = 0; i < 4; i++)
    {
        // Main container
        lv_obj_t * row = lv_obj_create(cont);
        lv_obj_set_width(row, 240);
        lv_obj_set_height(row, 25);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

        // Horizontal layout
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row,
                          LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_all(row, 2, 0);

        // Title
        lv_obj_t * label_title = lv_label_create(row);
        lv_obj_set_style_text_font(label_title, &lv_font_montserrat_12, 0);

        // Bar container
        lv_obj_t * bar_container = lv_obj_create(row);
        lv_obj_set_size(bar_container, 150, 20);
        lv_obj_set_style_bg_opa(bar_container, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(bar_container, 0, 0);
        lv_obj_set_style_radius(bar_container, 0, 0);
        lv_obj_clear_flag(bar_container, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_flex_flow(bar_container, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(bar_container,
                            LV_FLEX_ALIGN_START,
                            LV_FLEX_ALIGN_CENTER,
                            LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_all(bar_container, 0, 0);
        lv_obj_set_style_pad_column(bar_container, 2, 0);
        
        // Bar
        lv_obj_t * generic_bar;
        lv_obj_t * generic_bar_label;
        
        switch (i)
        {
            case 0:
            {
                lv_label_set_text(label_title, "UP");

                motor_up_thust_bar_ = lv_bar_create(bar_container);
                generic_bar = motor_up_thust_bar_;

                motor_up_thust_label_ = lv_label_create(bar_container);
                generic_bar_label = motor_up_thust_label_;
                break;
            }
            case 1:
            {
                lv_label_set_text(label_title, "DOWN");

                motor_down_thust_bar_ = lv_bar_create(bar_container);
                generic_bar = motor_down_thust_bar_;

                motor_down_thust_label_ = lv_label_create(bar_container);
                generic_bar_label = motor_down_thust_label_;
                break;
            }
            case 2:
            {
                lv_label_set_text(label_title, "LEFT");

                motor_left_thust_bar_ = lv_bar_create(bar_container);
                generic_bar = motor_left_thust_bar_;

                motor_left_thust_label_ = lv_label_create(bar_container);
                generic_bar_label = motor_left_thust_label_;
                break;
            }
            case 3:
            {
                lv_label_set_text(label_title, "RIGHT");

                motor_right_thust_bar_ = lv_bar_create(bar_container);
                generic_bar = motor_right_thust_bar_;

                motor_right_thust_label_ = lv_label_create(bar_container);
                generic_bar_label = motor_right_thust_label_;
                break;
            }

        }
        // Bar set
        lv_obj_set_size(generic_bar, 100, 14); 
        lv_bar_set_range(generic_bar, 0, 255);
        lv_bar_set_value(generic_bar, 0, LV_ANIM_OFF); // Default as 0

        // Bar number set
        lv_label_set_text_fmt(generic_bar_label, "%d", 0); // Default as 0
        lv_obj_set_style_text_font(generic_bar_label, &lv_font_montserrat_12, 0);
        lv_obj_set_style_pad_left(generic_bar_label, 4, 0);
    }
}

#endif