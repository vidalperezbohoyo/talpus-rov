#include <SoftwareSerial.h>
#include <TimerOne.h>

#include "DHT.h"

#include "Comms.h"

#define DHTTYPE DHT11

#define BEAT_RATE 1.0  // hz

const int CELL1_PIN = A0;

const int RX_PIN = 2;
const int TX_PIN = 3;

const int DHT_PIN = 5;


SoftwareSerial serial (RX_PIN, TX_PIN);

DHT dht(DHT_PIN, DHTTYPE);

void setup() {
  // Define pin modes for TX and RX
  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);
  pinMode(CELL1_PIN, INPUT);
 
  serial.begin(9600);
  dht.begin();
  unsigned long period = (1000000.0 / BEAT_RATE);
  Timer1.initialize(period);
  Timer1.attachInterrupt(sendBeat); 
}

void loop() {
  while (!serial.available()) {}; // Wait 
  
  Command command = (Command)serial.read();

  if (command == Command::TEMPERATURE_REQUEST) {
    sendTemperatureData();
  } else if (command == Command::BATTERY_REQUEST) {
    sendBatteryData();
  } else if (command == Command::HUMIDITY_REQUEST) {
    sendHumidityData();
  } if (command == Command::BEAT) {
    Serial.println("Beat <3");
  }
  
}


void sendBatteryData() {
  unsigned char * bytes;

  BatteryData battery = getBatteryData();

  noInterrupts();
  serial.write(Command::BATTERY_REPORT);
  bytes = (unsigned char*)&battery;
  serial.write(bytes, sizeof(BatteryData)); 
  interrupts();
}

void sendTemperatureData() {
  unsigned char * bytes;

  TemperatureData temperature = getTemperatureData();

  noInterrupts();
  serial.write(Command::TEMPERATURE_REPORT);
  bytes = (unsigned char*)&temperature;
  serial.write(bytes, sizeof(TemperatureData)); 
  interrupts();
}

void sendHumidityData() {
  unsigned char * bytes;

  HumidityData humidity = getHumidityData();

  noInterrupts();
  serial.write(Command::HUMIDITY_REPORT);
  bytes = (unsigned char*)&humidity;
  serial.write(bytes, sizeof(HumidityData)); 
  interrupts();
}

void sendBeat() {
  serial.write(Command::BEAT);
}

BatteryData getBatteryData() {

  BatteryData data;
  data.cellVoltage1 = analogRead(CELL1_PIN) * (5.0 / 1023.0);

  return data;
}

TemperatureData getTemperatureData() {
  // Takes about 250ms !
  TemperatureData data;
  data.temperature = dht.readTemperature();
  return data;
}

HumidityData getHumidityData() {
  // Takes about 250ms !
  HumidityData data;
  data.humidity = dht.readHumidity();
  return data;
}
