#pragma once
#include <Arduino.h>
#include "Defines.hpp"
#include "Protocol.hpp"

class RS485 {
public:
    // Singleton access
    static RS485& getInstance()
    {
        static RS485 instance;
        return instance;
    }

    void init(int rx_pin, int tx_pin, int de_re_pin);

    // Raw byte interface
    bool available();

    void send(uint8_t data);

    int read();

private:
    RS485() = default;
    RS485(const RS485&) = delete;
    RS485& operator=(const RS485&) = delete;

    void txMode();
    void rxMode();

    int de_re_pin_ = -1;
};
