#pragma once

#include <cstdint>

/* 
 * Pin definitions
 */

// OTA (wifi update)
#define PIN_OTA 6

// Status LED pins
#define PIN_RED    8
#define PIN_GREEN  9
#define PIN_BLUE   10

// RS485 pins
#define ROBOT_PIN_TX 21 // TX pin -> RS485 DI pin
#define ROBOT_PIN_RX 20 // RX pin <- RS485 RO pin
#define ROBOT_PIN_DE_RE 7 // MAX-458 DE/RE control pin

#define CONTROLLER_PIN_TX 21 // TX pin -> RS485 DI pin
#define CONTROLLER_PIN_RX 19 // RX pin <- RS485 RO pin
#define CONTROLLER_PIN_DE_RE 4 // MAX-458 DE/RE control pin

// Robot pins
#define PIN_BATTERY_ADC 5 // ADC pin for battery voltage reading. Voltage divider of 10k and 1k
#define PIN_LIGHTS 10 // Lights pin
#define PIN_MOTOR_1 0 // Up
#define PIN_MOTOR_2 1 // Down
#define PIN_MOTOR_3 2 // Left
#define PIN_MOTOR_4 3 // Right

// Controller pins
#define PIN_JOYSTICK_LEFT_X  0
#define PIN_JOYSTICK_LEFT_Y  1
#define PIN_JOYSTICK_RIGHT_X 2
#define PIN_JOYSTICK_RIGHT_Y 3

/* 
 * Other definitions
 */
#define ADC_MAX_VALUE 4095 // 12-bit ADC ESP32

#define JOYSTICK_DEADZONE 50 // In ADC units
#define JOYSTICK_CENTER (ADC_MAX_VALUE / 2)

#define OTA_SSID "ROV"
#define OTA_PASSWORD "123456789" // Use more than 8 characters to work!!!

#define EMERGENCY_RTL_TIMEOUT_MS 3000 // Time without commands to trigger emergency RTL

enum class MessageType : uint8_t
{
    CONTROL = 0b000,
    RESERVED_1 = 0b001,
    RESERVED_2 = 0b010,
    RESERVED_3 = 0b011,
    RESERVED_4 = 0b100,
    RESERVED_5 = 0b101,
    RESERVED_6 = 0b110,
    RESERVED_7 = 0b111
};

struct ControlMessage
{
    uint8_t motor_id; // 0-3
    uint8_t thrust;   // 0-255
};