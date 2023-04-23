#include <SoftwareSerial.h>

#include "Comms.h" 

const int RX_PIN = 2;
const int TX_PIN = 3;

SoftwareSerial serial (RX_PIN, TX_PIN);

void setup() {
  // Define pin modes for TX and RX
  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);

  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);

  Serial.begin(9600);
  serial.begin(9600);
}

void loop() {

  if (serial.available()) {
    Command command = (Command)serial.read();

    if (command == Command::TEMPERATURE_REPORT) {
      receiveTemperatureData();
    } else if (command == Command::BATTERY_REPORT) {
      receiveBatteryData();
    } else if (command == Command::HUMIDITY_REPORT) {
      receiveHumidityData();
    } else if (command == Command::BEAT) {
      Serial.println("Beat <3");
      serial.write(Command::TEMPERATURE_REQUEST);
    }
  }


  
  
  if (!digitalRead(8)) {
    serial.write(Command::BATTERY_REQUEST);
    serial.write(Command::HUMIDITY_REQUEST);
    delay(500);
  }
  if (!digitalRead(9)) {
    serial.write(Command::TEMPERATURE_REQUEST);
    delay(500);
  }

 
}

void receiveTemperatureData() {
  
  TemperatureData data;
  int structureSize = (int)sizeof(TemperatureData);
  while (serial.available() < structureSize);

    unsigned char* bytes = (unsigned char*)&data;
    for (int i = 0; i < structureSize; i++) {
      bytes[i] = serial.read();
    }

   data = *(TemperatureData*)bytes;

   Serial.print("T: ");
   Serial.print(data.temperature);
   Serial.println("ºC");
}

void receiveHumidityData() {
  
  HumidityData data;
  int structureSize = (int)sizeof(HumidityData);
  while (serial.available() < structureSize);

    unsigned char* bytes = (unsigned char*)&data;
    for (int i = 0; i < structureSize; i++) {
      bytes[i] = serial.read();
    }

   data = *(HumidityData*)bytes;

   Serial.print("H: ");
   Serial.print(data.humidity);
   Serial.println("%");
}

void receiveBatteryData() {
  BatteryData data;
  int structureSize = (int)sizeof(BatteryData);
  while (serial.available() < structureSize);

    unsigned char* bytes = (unsigned char*)&data;
    for (int i = 0; i < structureSize; i++) {
      bytes[i] = serial.read();
    }

   data = *(BatteryData*)bytes;

   Serial.print("Cell 1: ");
   Serial.print(data.cellVoltage1);
   Serial.println("V");
}
