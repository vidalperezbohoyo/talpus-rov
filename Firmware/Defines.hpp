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


/* 
 * Other definitions
 */
#define ADC_MAX_VALUE 4095 // 12-bit ADC ESP32

#define JOYSTICK_DEADZONE 50 // In ADC units
#define JOYSTICK_CENTER (ADC_MAX_VALUE / 2)

#define OTA_SSID "ROV"
#define OTA_PASSWORD "123456789" // Use more than 8 characters to work!!!

#define EMERGENCY_RTL_TIMEOUT_MS 3000 // Time without commands to trigger emergency RTL

#define REQUEST_RETRIES 3 // How many times to retry a request

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))

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
*/
const float VOLTAGE_TABLE[32] = {
  9.00, 9.30, 9.60, 9.85,
  10.05, 10.25, 10.45, 10.65,
  10.80, 10.95, 11.05, 11.15,
  11.25, 11.35, 11.40, 11.45,
  11.50, 11.55, 11.60, 11.70,
  11.80, 11.90, 12.00, 12.10,
  12.20, 12.30, 12.35, 12.40,
  12.45, 12.50, 12.55, 12.60
};

const int PERCENT_TABLE[32] = {
   0,  3,  6, 10,
  13, 16, 19, 23,
  26, 29, 32, 35,
  39, 42, 45, 48,
  52, 55, 58, 61,
  65, 68, 71, 74,
  77, 81, 84, 87,
  90, 94, 97, 100
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