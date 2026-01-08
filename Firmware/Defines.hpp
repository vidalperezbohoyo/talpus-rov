#pragma once

#define ADC_MAX_VALUE 4095 // 12-bit ADC ESP32

#define SYNC_BYTE 0xAA

// Pin definitions

#define PIN_DE_RE 7 // MAX-458 DE/RE control pin

#define PIN_BATTERY_ADC 4 // ADC pin for battery voltage reading. Voltage divider of 10k and 1k

enum CommandCode
{
    CMD_NONE = 0,
    CMD_CONTROL,
    CMD_LIGHTS,
    CMD_REQUEST_BATTERY,
    CMD_REQUEST_LIVE_STATUS
};

enum ResponseCode
{
    RES_UNKNOWN = 0,
    RES_OK,
    RES_FAILURE
};

struct Command
{
    uint8_t code; // CommandCode
    uint8_t param1;
    uint8_t param2;
    uint8_t param3;
    uint8_t param4;
};

struct Response
{
    uint8_t code; // ResponseCode
    uint8_t value;
};