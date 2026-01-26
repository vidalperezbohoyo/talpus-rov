#pragma once
#include "Defines.hpp"

#include "Arduino.h"

class Protocol
{
public:
    
    static MessageType getMessageType(const uint8_t& byte);

    static uint8_t pack(const ControlMessage& message);
    static uint8_t pack(const BatteryResponseMessage& message);
    static uint8_t pack(const BatteryRequestMessage& message);
    static uint8_t pack(const LightsMessage& message);

    static bool unpack(const uint8_t& byte, ControlMessage& message);
    static bool unpack(const uint8_t& byte, BatteryResponseMessage& message);
    static bool unpack(const uint8_t& byte, BatteryRequestMessage& message);
    static bool unpack(const uint8_t& byte, LightsMessage& message);

};