#pragma once

enum Command {
  ALERT,
  ALERT_REQUEST,
  BEAT,
  BEAT_REQUEST,
  BATTERY,
  BATTERY_REQUEST,
  TEMPERATURE,
  TEMPERATURE_REQUEST,
  HUMIDITY,
  HUMIDITY_REQUEST,
  SPEED
};

enum Alert {
  NO_ALERT,
  LOW_BATTERY,
  OVER_TEMPERATURE
};

struct BatteryData {
  float cellVoltage1;
};

struct SpeedData {
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
