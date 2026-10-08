#include "Battery.hpp"

void Battery::init()
{
#if defined(ARDUINO_ESP32_DEV) // Only for ESP32
    pinMode(CONTROLLER_PIN_BAT_IN, INPUT);
#else // For ROV
    pinMode(ROBOT_PIN_BAT_IN, INPUT);
#endif
}

BatteryInformation Battery::info()
{
    BatteryInformation battery_info;

    uint16_t adc = readADC();

    // Map ADC to voltage, simple map without 

    // ACD lower than minimum
#if defined(ARDUINO_ESP32_DEV)
    if (adc <= ADC_TABLE_ESP32_DEV[0]) 
#else
    if (adc <= ADC_TABLE_ESP32_C3[0])
#endif
    {
        battery_info.voltage = VOLTAGE_TABLE[0];
        battery_info.percentage = PERCENT_TABLE[0];
        return battery_info; // Minimum value
    }
    // ADC higher than maximum
#if defined(ARDUINO_ESP32_DEV)
    else if (adc >= ADC_TABLE_ESP32_DEV[31]) 
#else
    else if (adc >= ADC_TABLE_ESP32_C3[31])
#endif
    {
        battery_info.voltage = VOLTAGE_TABLE[31];
        battery_info.percentage = PERCENT_TABLE[31];
        return battery_info; // Maximum value
    }

    // else, find in table
    for (int i = 0; i < 32; i++)
    {
#if defined(ARDUINO_ESP32_DEV)
        if (ADC_TABLE_ESP32_DEV[i] >= adc)
#else
        if (ADC_TABLE_ESP32_C3[i] >= adc)
#endif
        {
            battery_info.voltage = VOLTAGE_TABLE[i];
            battery_info.percentage = PERCENT_TABLE[i];
            break; // Filled
        }
    }

    return battery_info;
}

float Battery::convertPercentageToVoltage(uint8_t percentage)
{
    // Find in table the closest percentage
    for (int i = 0; i < 32; i++)
    {
        if (PERCENT_TABLE[i] >= percentage)
        {
            return VOLTAGE_TABLE[i];
        }
    }
    return 0.0f; // Default value if percentage is not found
}

uint16_t Battery::readADC()
{
#if defined(ARDUINO_ESP32_DEV) // Only for ESP32
    int new_adc = analogRead(CONTROLLER_PIN_BAT_IN); // Discarded
    delay(10); // Small delay to stabilize ADC reading
    new_adc = analogRead(CONTROLLER_PIN_BAT_IN);
#else // For ROV
    int new_adc = analogRead(ROBOT_PIN_BAT_IN);
    // No delay to avoid protocol fail at sync
#endif

    // Serial.println("[Battery::readADC] New ADC reading: " + String(new_adc));

    return static_cast<uint16_t>(new_adc);
}