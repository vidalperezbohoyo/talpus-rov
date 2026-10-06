#pragma once

#include <cstdint>

/* 
 * Pin definitions
 */

// OTA (wifi update)
#define PIN_OTA 6

// Status LED pins
#define PIN_STATUS_LED  5

// RS485 pins
#define ROBOT_PIN_TX 21 // TX pin -> RS485 DI pin
#define ROBOT_PIN_RX 20 // RX pin <- RS485 RO pin
#define ROBOT_PIN_DE_RE 7 // MAX-458 DE/RE control pin

#define CONTROLLER_PIN_TX 27 // TX pin -> RS485 DI pin
#define CONTROLLER_PIN_RX 22 // RX pin <- RS485 RO pin
#define CONTROLLER_PIN_DE_RE 4 // MAX-458 DE/RE control pin

// Robot pins
#define PIN_LIGHTS 8 // Lights pin
#define PIN_MOTOR_1 0 // Up
#define PIN_MOTOR_2 1 // Down
#define PIN_MOTOR_3 2 // Left
#define PIN_MOTOR_4 3 // Right

// Battery pins
#define CONTROLLER_PIN_BAT_IN 35
#define ROBOT_PIN_BAT_IN 4

/* 
 * Other definitions
 */
#define OTA_SSID "ROV"
#define OTA_PASSWORD "123456789" // Use more than 8 characters to work!!!

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))

#define JOYSTICK_DEADZONE 10 // Threshold for the joystick center position

enum class MessageType : uint8_t
{
    EMPTY = 0b000,
    CONTROL = 0b001,
    REQUEST_BATTERY = 0b010,
    RESPONSE_BATTERY = 0b011,
    LIGHTS = 0b100,
    OTA_UPDATE = 0b101,
    RESERVED_6 = 0b110,
    RESERVED_7 = 0b111
};

struct ControlMessage
{
    uint8_t motor_id; // 0-3
    uint8_t thrust;   // 0-255
};

struct OTAUpdateMessage
{
    // Empty, just the type is needed to trigger OTA mode
};

struct BatteryRequestMessage
{
    // Empty
};

struct BatteryResponseMessage
{
    uint8_t percentage;
};

struct LightsMessage
{
    uint8_t intensity; // 0-255
};

/*
 Battery voltage to percentage mapping
 3S Li-ion: 10.5V (0%) to 12.6V (100%)
*/

const uint16_t ADC_TABLE_ESP32_DEV[32] = {
  2937, 2995, 3055, 3122,
  3164, 3207, 3254, 3302,
  3347, 3395, 3445, 3496,
  3550, 3602, 3633, 3658,
  3684, 3707, 3735, 3760,
  3792, 3814, 3839, 3860,
  3888, 3901, 3922, 3943,
  3952, 3957, 3965, 3970
};

const uint16_t ADC_TABLE_ESP32_C3[32] = {
  2954, 2991, 3036, 3084,
  3112, 3145, 3168, 3193,
  3226, 3258, 3287, 3320,
  3341, 3385, 3390, 3402,
  3418, 3435, 3448, 3460,
  3470, 3483, 3501, 3502,
  3510, 3518, 3530, 3550,
  3556, 3558, 3560, 3561

};

const float VOLTAGE_TABLE[32] = {
  10.50, 10.65, 10.80, 10.95,
  11.05, 11.15, 11.25, 11.35,
  11.45, 11.55, 11.65, 11.75,
  11.85, 11.95, 12.00, 12.05,
  12.10, 12.15, 12.20, 12.25,
  12.30, 12.34, 12.38, 12.42,
  12.46, 12.49, 12.52, 12.55,
  12.57, 12.58, 12.59, 12.60
};

const uint8_t PERCENT_TABLE[32] = {
   0,  3,  6, 10,
  13, 16, 19, 23,
  26, 29, 32, 35,
  39, 42, 45, 48,
  52, 55, 58, 61,
  65, 68, 71, 80,
  85, 90, 93, 95,
  97, 98, 99, 100
};

/*
    Struct for UI
*/
enum class BatteryType
{
    ROV,
    CONTROLLER,
    DUALSHOCK
};

struct MotorInformation
{
    uint8_t motor_id; // 0-3
    uint8_t thrust;   // 0-255
};

struct BatteryInformation
{
    BatteryType type;
    uint8_t percentage;
    float voltage; // Not for DualShock
    bool charging; // Only for DualShock
};

struct LightsInformation
{
    uint8_t intensity; // 0-255
};