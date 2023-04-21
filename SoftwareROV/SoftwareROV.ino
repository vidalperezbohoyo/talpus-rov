#include <SoftwareSerial.h>
#include "DHT.h"

#include "Comms.h"


#define DHTTYPE DHT11

#define CELL2_VOLTAGE_DIVIDER_R1 100  
#define CELL2_VOLTAGE_DIVIDER_R2 100
#define CELL3_VOLTAGE_DIVIDER_R1 100
#define CELL3_VOLTAGE_DIVIDER_R2 100

#define SENDER_PERIOD 2000  // milliseconds

const int CELL1_PIN = A0;
const int CELL2_PIN = A1;
const int CELL3_PIN = A2;

const int RX_PIN = 2;
const int TX_PIN = 3;

const int DHT_PIN = 5;


SoftwareSerial serial (RX_PIN, TX_PIN);

DHT dht(DHT_PIN, DHTTYPE);

void setup() {
  // Define pin modes for TX and RX
  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);
  
  serial.begin(9600);
  dht.begin();
}

void loop() {
  serialSender();
  delay(2000);
}

void serialSender() {
  unsigned char * bytes;

  //BatteryData battery = getBatteryData();
  TemperatureHumidityData temperatureHumidity = getTemperatureHumidityData();
  /*
  // Sending battery data
  Serial.write(Command::BATTERY_DATA);
  bytes = (unsigned char*)&battery;
  Serial.write(bytes, sizeof(BatteryData));  
  */
  // Sending temperature and humidity data
  serial.write(Command::TEMPERATURE_HUMIDITY_DATA);
  bytes = (unsigned char*)&temperatureHumidity;
  serial.write(bytes, sizeof(TemperatureHumidityData));    

  // Sending beat (ROV is alive!)
  //Serial.write(Command::BEAT);
}

// ROV HAL

BatteryData getBatteryData() {

  BatteryData data;
  data.cellVoltage1 = analogRead(CELL1_PIN) * (5.0 / 1023.0);
  data.cellVoltage2 = analogRead(CELL2_PIN) * (5.0 / 1023.0) * ((CELL2_VOLTAGE_DIVIDER_R1 + CELL2_VOLTAGE_DIVIDER_R2) / CELL2_VOLTAGE_DIVIDER_R2);
  data.cellVoltage3 = analogRead(CELL3_PIN) * (5.0 / 1023.0) * ((CELL3_VOLTAGE_DIVIDER_R1 + CELL3_VOLTAGE_DIVIDER_R2) / CELL3_VOLTAGE_DIVIDER_R2);
  
  return data;
}

TemperatureHumidityData getTemperatureHumidityData() {
  // Takes about 500ms !
  TemperatureHumidityData data;
  data.temperature = dht.readTemperature();
  data.humidity = dht.readHumidity();
  return data;
}

