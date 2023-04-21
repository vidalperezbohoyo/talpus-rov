#pragma once

enum Command {
  ALERT,
  BEAT,
  BATTERY_REPORT,
  BATTERY_REQUEST,
  TEMPERATURE_REPORT,
  TEMPERATURE_REQUEST,
  HUMIDITY_REPORT,
  HUMIDITY_REQUEST
};

enum Alert {
  LOW_BATTERY,
  OVER_TEMPERATURE
};

struct BatteryData {
  float cellVoltage1;
};

struct MoveData {
  unsigned char leftMotorThrust;
  unsigned char rightMotorThrust;
  unsigned char upMotorThrust;
  unsigned char downMotorThrust;
};

struct TemperatureData {
  float temperature;
};

struct HumidityData {
  float humidity;
};

struct AlertData {
  Alert alert;
};
