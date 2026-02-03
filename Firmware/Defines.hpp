#pragma once

#include <cstdint>

/* 
 * Pin definitions
 */

// OTA (wifi update)
#define PIN_OTA 6

// Status LED pins
#define PIN_GREEN  9
#define PIN_BLUE   10

// RS485 pins
#define ROBOT_PIN_TX 21 // TX pin -> RS485 DI pin
#define ROBOT_PIN_RX 20 // RX pin <- RS485 RO pin
#define ROBOT_PIN_DE_RE 7 // MAX-458 DE/RE control pin

#define CONTROLLER_PIN_TX 27 // TX pin -> RS485 DI pin
#define CONTROLLER_PIN_RX 22 // RX pin <- RS485 RO pin
#define CONTROLLER_PIN_DE_RE 4 // MAX-458 DE/RE control pin

// Robot pins
#define PIN_BATTERY_ADC 5 // ADC pin for battery voltage reading. Voltage divider of 10k and 1k
#define PIN_LIGHTS 8 // Lights pin
#define PIN_MOTOR_1 0 // Up
#define PIN_MOTOR_2 1 // Down
#define PIN_MOTOR_3 2 // Left
#define PIN_MOTOR_4 3 // Right

// Battery pins
#define CONTROLLER_PIN_BAT_IN 35

/* 
 * Other definitions
 */
#define OTA_SSID "ROV"
#define OTA_PASSWORD "123456789" // Use more than 8 characters to work!!!

#define EMERGENCY_RTL_TIMEOUT_MS 3000 // Time without commands to trigger emergency RTL

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))

// Battery
#define ADC_MAX_VALUE 4095.0f // 12-bit ADC ESP32
#define ADC_REF_VOLTAGE 3.3f // Reference voltage for ADC
#define BATTERY_VOLTAGE_DIVIDER_RATIO ((10000.f + 3300.f) / 3300.f) // R1=10k, R2=3.3k Voltage divider ratio for battery voltage reading
#define CONTROLLER_BATTERY_ADC_ADJUST 1.0885f // Adjust factor for controller battery voltage reading
#define ROV_BATTERY_ADC_ADJUST 1.257f // Adjust factor for ROV battery voltage reading

enum class MessageType : uint8_t
{
    EMPTY = 0b000,
    CONTROL = 0b001,
    REQUEST_BATTERY = 0b010,
    RESPONSE_BATTERY = 0b011,
    LIGHTS = 0b100,
    RESERVED_5 = 0b101,
    RESERVED_6 = 0b110,
    RESERVED_7 = 0b111
};

struct ControlMessage
{
    uint8_t motor_id; // 0-3
    uint8_t thrust;   // 0-255
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

const uint16_t ADC_TABLE[32] = {
  2937, 2995, 3055, 3122,
  3164, 3207, 3254, 3302,
  3347, 3395, 3445, 3496,
  3550, 3602, 3633, 3658,
  3684, 3707, 3735, 3760,
  3792, 3814, 3839, 3860,
  3888, 3901, 3922, 3943,
  3952, 3957, 3965, 3970
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