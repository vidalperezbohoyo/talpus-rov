#include "RS485.hpp"

void RS485::init(int rx_pin, int tx_pin, int de_re_pin)
{
    // Configure DE/RE pin for RS485 transceiver direction control
    pinMode(de_re_pin, OUTPUT);

    // Start UART with defined pins and configuration
    Serial1.begin(115200, SERIAL_8N1, rx_pin, tx_pin);

    de_re_pin_ = de_re_pin;

    // Default to receive mode
    rxMode();
}

bool RS485::available()
{
    // Ensure receiver is enabled
    rxMode();
    return Serial1.available() > 0;
}

void RS485::send(uint8_t data)
{
    // Enable transmitter
    txMode();

    // Send byte
    Serial1.write(data);
    Serial1.flush(); // Wait for transmission to complete
}

int RS485::read()
{
    // Ensure receiver is enabled
    rxMode();

    if (Serial1.available() > 0)
    {
        return Serial1.read();
    }
    else
    {
        return -1; // No data available
    }
}

void RS485::txMode()
{
    // Enable transmitter, disable receiver
    digitalWrite(de_re_pin_, HIGH);
}

void RS485::rxMode()
{
    // Enable receiver, disable transmitter
    digitalWrite(de_re_pin_, LOW);
}
