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

    draw_buf_ = new uint16_t[DRAW_BUF_SIZE];

    lv_display_t * disp;
    disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf_, DRAW_BUF_SIZE);

    createDashboard();
}

void UI::update(const BatteryInformation& battery_info)
{
    lv_obj_t* arc;

    switch (battery_info.type)
    {
        case BatteryType::ROV:
            lv_label_set_text_fmt(rov_battery_label_, "%d%%", battery_info.percentage);
            arc = rov_battery_arc_;
            break;
        case BatteryType::CONTROLLER:
            lv_label_set_text_fmt(controller_battery_label_, "%d%%", battery_info.percentage);
            arc = controller_battery_arc_;
            break;
        case BatteryType::DUALSHOCK: 
            if (battery_info.charging)
            {
                lv_label_set_text_fmt(dualshock_battery_label_, "%d%%\nCharging", battery_info.percentage);
            }
            else
            {
                lv_label_set_text_fmt(dualshock_battery_label_, "%d%%", battery_info.percentage);
            }
            arc = dualshock_battery_arc_;
            break;
        default:
            return; // Unknown type
    }

    lv_arc_set_value(arc, battery_info.percentage);

    // Change color based on percentage
    if(battery_info.percentage > 60)
    {
        lv_obj_set_style_arc_color(arc, lv_color_make(0, 255, 0), LV_PART_INDICATOR);
    }
    else if(battery_info.percentage > 30)
    {
        lv_obj_set_style_arc_color(arc, lv_color_make(255, 165, 0), LV_PART_INDICATOR);
    }
    else
    {
        lv_obj_set_style_arc_color(arc, lv_color_make(255, 0, 0), LV_PART_INDICATOR);
    }

    lv_obj_set_style_arc_opa(arc, LV_OPA_COVER, LV_PART_INDICATOR);

    lv_obj_set_style_arc_color(
        arc,
        lv_color_make(40, 40, 40),
        LV_PART_MAIN
    );
    lv_obj_set_style_arc_opa(arc, LV_OPA_30, LV_PART_MAIN);
}

void UI::update(const LightsInformation& lights_info)
{
    lv_label_set_text_fmt(lights_label_, "%d%%", lights_info.intensity * 100 / 255);
    lv_arc_set_value(lights_arc_, lights_info.intensity * 100 / 255);

    lv_obj_set_style_arc_opa(lights_arc_, LV_OPA_COVER, LV_PART_INDICATOR);

    lv_obj_set_style_arc_color(
        lights_arc_,
        lv_color_make(40, 40, 40),
        LV_PART_MAIN
    );
    lv_obj_set_style_arc_opa(lights_arc_, LV_OPA_30, LV_PART_MAIN);
}

void UI::refresh()
{
    // Compute how many ms elapsed since last call
    static uint32_t last_refresh = 0;
    uint32_t now = millis();
    uint32_t elapsed = now - last_refresh;
    last_refresh = now;

    lv_tick_inc(elapsed);
    lv_timer_handler(); /* let the GUI do its work */
    //Serial.println("[DEBUG] Refreshing screen");
}

void UI::createDashboard()
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
    lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(lv_scr_act(), lv_color_white(), LV_PART_MAIN);

    createMainContainer();
}

void UI::createMainContainer()
{
    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, 320, 240);
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 0);

    // CONTENEDOR PRINCIPAL: columnas (2 filas)
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont,
                          LV_FLEX_ALIGN_SPACE_EVENLY,
                          LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(cont, lv_color_black(), 0);

    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    for (int row = 0; row < 2; row++)
    {
        // FILA
        lv_obj_t * row_cont = lv_obj_create(cont);
        lv_obj_set_size(row_cont, 320, 120);

        lv_obj_set_flex_flow(row_cont, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row_cont,
                              LV_FLEX_ALIGN_SPACE_EVENLY,
                              LV_FLEX_ALIGN_CENTER,
                              LV_FLEX_ALIGN_CENTER);

        lv_obj_set_style_bg_opa(row_cont, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(row_cont, lv_color_black(), 0);
        lv_obj_set_style_border_width(row_cont, 0, 0);
        lv_obj_clear_flag(row_cont, LV_OBJ_FLAG_SCROLLABLE);

        for (int col = 0; col < 2; col++)
        {
            int idx = row * 2 + col;

            // CELDA
            lv_obj_t * cell = lv_obj_create(row_cont);
            lv_obj_set_size(cell, 140, 110);
            lv_obj_set_style_bg_opa(cell, LV_OPA_COVER, 0);
            lv_obj_set_style_bg_color(cell, lv_color_black(), 0);
            lv_obj_set_style_border_width(cell, 0, 0);
            lv_obj_clear_flag(cell, LV_OBJ_FLAG_SCROLLABLE);

            lv_obj_set_flex_flow(cell, LV_FLEX_FLOW_COLUMN);
            lv_obj_set_flex_align(cell,
                                  LV_FLEX_ALIGN_CENTER,
                                  LV_FLEX_ALIGN_CENTER,
                                  LV_FLEX_ALIGN_CENTER);

            // TÍTULO
            lv_obj_t * title = lv_label_create(cell);
            lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
            lv_obj_set_style_text_opa(title, LV_OPA_COVER, LV_PART_MAIN);
            lv_obj_set_style_text_color(title, lv_color_white(), LV_PART_MAIN);

            // ARC + LABEL
            lv_obj_t * arc = NULL;
            lv_obj_t * label = NULL;

            switch (idx)
            {
                case 0:
                    lv_label_set_text(title, "ROV");
                    rov_battery_arc_ = lv_arc_create(cell);
                    arc = rov_battery_arc_;
                    rov_battery_label_ = lv_label_create(arc);
                    label = rov_battery_label_;
                    break;

                case 1:
                    lv_label_set_text(title, "GroundBox");
                    controller_battery_arc_ = lv_arc_create(cell);
                    arc = controller_battery_arc_;
                    controller_battery_label_ = lv_label_create(arc);
                    label = controller_battery_label_;
                    break;

                case 2:
                    lv_label_set_text(title, "DualShock4");
                    dualshock_battery_arc_ = lv_arc_create(cell);
                    arc = dualshock_battery_arc_;
                    dualshock_battery_label_ = lv_label_create(arc);
                    label = dualshock_battery_label_;
                    break;

                case 3:
                    lv_label_set_text(title, "Lights");
                    lights_arc_ = lv_arc_create(cell);
                    arc = lights_arc_;
                    lights_label_ = lv_label_create(arc);
                    label = lights_label_;
                    break;
            }

            // CONFIGURACIÓN DEL ARC
            lv_obj_set_size(arc, 80, 80);
            lv_arc_set_range(arc, 0, 100);
            lv_arc_set_value(arc, 0);
            lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
            lv_arc_set_bg_angles(arc, 135, 45);
            lv_obj_set_style_arc_opa(arc, LV_OPA_COVER, LV_PART_INDICATOR);
            
            // LABEL CENTRAL
            lv_label_set_text(label, "?");
            lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
            lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
            lv_obj_center(label);
            lv_obj_set_style_text_opa(label, LV_OPA_COVER, LV_PART_MAIN);
            lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
        }
    }
}

#endif