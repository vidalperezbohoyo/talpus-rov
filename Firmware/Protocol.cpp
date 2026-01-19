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
        Serial.print("[ERROR] Invalid header on ControlMessage unpack: "); Serial.println(static_cast<int>(getMessageType(byte)));
        return false; // Not a control message
    }

    // Extract motor ID (next 2 bits)
    message.motor_id = static_cast<uint8_t>((byte >> 3) & 0b00000011);

    // Extract thrust value (last 3 bits)
    uint8_t zero_to_seven = static_cast<uint8_t>(byte & 0b00000111);

    switch (zero_to_seven)
    {
        case 0:
            message.thrust = 0;
            break;
        case 1:
            message.thrust = 32;
            break;
        case 2:
            message.thrust = 64;
            break;
        case 3:
            message.thrust = 96;
            break;
        case 4:
            message.thrust = 128;
            break;
        case 5:
            message.thrust = 160;
            break;
        case 6:
            message.thrust = 192;
            break;
        case 7:
            message.thrust = 255;
            break;
    }

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

uint8_t Protocol::pack(const BatteryRequestMessage& message)
{
    uint8_t byte = 0;

    // Set message type to REQUEST_BATTERY (first 3 bits)
    byte |= (static_cast<uint8_t>(MessageType::REQUEST_BATTERY) & 0b00000111) << 5;

    // No additional data for battery request, remaining bits are 0

    return byte;
}

bool Protocol::unpack(const uint8_t& byte, BatteryResponseMessage& message)
{
    if (getMessageType(byte) != MessageType::RESPONSE_BATTERY)
    {
        Serial.print("[ERROR] Invalid header on BatteryResponseMessage unpack. Header: "); Serial.print(static_cast<int>(getMessageType(byte))); Serial.print(". Full msg: "); Serial.println(static_cast<int>(byte));
        return false; // Not a battery response message
    }

    // Extract battery percentage (last 5 bits)
    // 2⁵ = 32 levels (0-31), map to 0-100%
    message.percentage = static_cast<uint8_t>(byte & 0b00011111);
    message.percentage = static_cast<uint8_t>((message.percentage * 100) / 31);

    return true;
}

uint8_t Protocol::pack(const BatteryResponseMessage& message)
{
    uint8_t byte = 0;

    // Set message type to RESPONSE_BATTERY (first 3 bits)
    byte |= (static_cast<uint8_t>(MessageType::RESPONSE_BATTERY) & 0b00000111) << 5;

    // Set battery percentage (last 5 bits)
    // Map 0-100% to 0-31
    uint8_t scaled_percentage = static_cast<uint8_t>((message.percentage * 31) / 100);
    byte |= (scaled_percentage & 0b00011111);

    return byte;
}

bool Protocol::unpack(const uint8_t& byte, BatteryRequestMessage& message)
{
    if (getMessageType(byte) != MessageType::REQUEST_BATTERY)
    {
        Serial.print("[ERROR] Invalid header on BatteryRequestMessage unpack: "); Serial.println(static_cast<int>(getMessageType(byte)));
        return false; // Not a battery request message
    }

    // No additional data to extract for battery request

    return true;
}