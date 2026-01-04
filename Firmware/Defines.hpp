#pragma once

#define SYNC_BYTE 0xAA

#define CONTROLLER_POLL_RATE 

enum CommandCode
{
    CMD_NONE = 0,
    CMD_CONTROL,
    CMD_LIGHTS,
    CMD_BATTERY
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