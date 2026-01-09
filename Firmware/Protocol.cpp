#include "Protocol.hpp"

MessageType Protocol::getMessageType(const uint8_t& byte)
{
    uint8_t type_bits = (byte >> 5) & 0b00000111; // Extract first 3 bits

    return static_cast<MessageType>(type_bits);
}

bool Protocol::unpack(const uint8_t& byte, ControlMessage& message)
{
    if (getMessageType(byte) != MessageType::CONTROL)
    {
        return false; // Not a control message
    }

    // Extract motor ID (next 2 bits)
    message.motor_id = static_cast<uint8_t>((byte >> 3) & 0b00000011);

    // Extract thrust value (last 3 bits)
    message.thrust = static_cast<uint8_t>(byte & 0b00000111) * 32; // Scale to 0-255

    return true;
}

uint8_t Protocol::pack(const ControlMessage& message)
{
    uint8_t byte = 0;

    // Set message type to CONTROL (first 3 bits)
    byte |= (static_cast<uint8_t>(MessageType::CONTROL) & 0b00000111) << 5;

    // Set motor ID (next 2 bits)
    byte |= (message.motor_id & 0b00000011) << 3;

    // Set thrust value (last 3 bits), scaled down to 0-7
    uint8_t scaled_thrust = message.thrust / 32; // Scale from 0-255 to 0-7
    byte |= (scaled_thrust & 0b00000111);

    return byte;
}