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

    Serial.println("[Battery::info] ADC reading: " + String(adc));

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
    static float filtered_adc = 0.0f; // Memory for IIR filter

    const float alpha = 0.3f; // IIR filter coefficient (0 < alpha < 1) The lower the value, the smoother the result

#if defined(ARDUINO_ESP32_DEV) // Only for ESP32
    float new_adc = static_cast<float>(analogRead(CONTROLLER_PIN_BAT_IN)); // Discarded
    delay(10); // Small delay to stabilize ADC reading
    new_adc = static_cast<float>(analogRead(CONTROLLER_PIN_BAT_IN));
#else // For ROV
    float new_adc = static_cast<float>(analogRead(ROBOT_PIN_BAT_IN));
    // No delay to avoid protocol fail at sync
#endif

    // Apply correction multiplier
#if defined(ARDUINO_ESP32_DEV) // Only for ESP32
    new_adc *= CONTROLLER_BATTERY_ADC_MULTIPLIER;
#else
    new_adc *= ROBOT_BATTERY_ADC_MULTIPLIER;
#endif

    // Check if first time
    if (filtered_adc < 0.1f) 
    {
        filtered_adc = new_adc; // Initialize filter
    }

    // Apply IIR filter
    filtered_adc = alpha * new_adc + (1.0f - alpha) * filtered_adc;

    return static_cast<uint16_t>(filtered_adc);
}