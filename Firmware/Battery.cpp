#include "Battery.hpp"

void Battery::init()
{
    pinMode(PIN_BATTERY_ADC, INPUT);
}

float Battery::getVoltage()
{
    int adc_value = analogRead(PIN_BATTERY_ADC);
    float voltage = (adc_value / (float)ADC_MAX_VALUE) * 3.3f * (11.0f);
    return voltage;
}

uint8_t Battery::getPercentage()
{
    float voltage = getVoltage();

    // Simple linear approximation between 11.0V (0%) and 12.6V (100%)
    if (voltage >= 12.6f)
    {
        return 100;
    }
    else if (voltage <= 11.0f)
    {
        return 0;
    }
    else
    {
        return (unsigned int)(((voltage - 11.0f) / (12.6f - 11.0f)) * 100.0f);
    }
}
