#include "Battery.hpp"

void Battery::init()
{
    pinMode(CONTROLLER_PIN_BAT_IN, INPUT);
}

BatteryInformation Battery::info()
{
    BatteryInformation battery_info;

    uint16_t adc = readADC();

    // Map ADC to voltage, simple map without 

    // ACD lower than minimum
    if (adc <= ADC_TABLE[0]) 
    {
        battery_info.voltage = VOLTAGE_TABLE[0];
        battery_info.percentage = PERCENT_TABLE[0];
        return battery_info; // Minimum value
    }
    // ADC higher than maximum
    else if (adc >= ADC_TABLE[31]) 
    {
        battery_info.voltage = VOLTAGE_TABLE[31];
        battery_info.percentage = PERCENT_TABLE[31];
        return battery_info; // Maximum value
    }

    // else, find in table
    for (int i = 0; i < 32; i++)
    {
        if (ADC_TABLE[i] >= adc)
        {
            battery_info.voltage = VOLTAGE_TABLE[i];
            battery_info.percentage = PERCENT_TABLE[i];
            break; // Filled
        }
    }

    return battery_info;
}

uint16_t Battery::readADC()
{
    static float filtered_adc = 0.0f; // Memory for IIR filter

    const float alpha = 0.3f; // IIR filter coefficient (0 < alpha < 1) The lower the value, the smoother the result

    float new_adc = static_cast<float>(analogRead(CONTROLLER_PIN_BAT_IN)); // Discarded
    delay(10); // Small delay to stabilize ADC reading
    new_adc = static_cast<float>(analogRead(CONTROLLER_PIN_BAT_IN));

    // Check if first time
    if (filtered_adc < 0.1f) 
    {
        filtered_adc = new_adc; // Initialize filter
    }

    // Apply IIR filter
    filtered_adc = alpha * new_adc + (1.0f - alpha) * filtered_adc;

    return static_cast<uint16_t>(filtered_adc);
}