#pragma once
#include "Defines.hpp"

class Protocol
{
public:
    
    static MessageType getMessageType(const uint8_t& byte);

    static uint8_t pack(const ControlMessage& message);

    static bool unpack(const uint8_t& byte, ControlMessage& message);
};