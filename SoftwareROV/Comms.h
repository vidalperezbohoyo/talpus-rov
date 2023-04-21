#pragma once

enum Command {
  ALERT,
  BEAT,
  BATTERY_DATA,
  MOVE,
  TEMPERATURE_HUMIDITY_DATA

};

enum Alert {
  LOW_BATTERY,
  OVER_TEMPERATURE
};

struct BatteryData {
  float cellVoltage1;
  float cellVoltage2;
  float cellVoltage3;
};

struct MoveData {
  unsigned char leftMotorThrust;
  unsigned char rightMotorThrust;
  unsigned char upMotorThrust;
  unsigned char downMotorThrust;
};

struct TemperatureHumidityData {
  float temperature;
  float humidity;
};

struct AlertData {
  Alert alert;
};