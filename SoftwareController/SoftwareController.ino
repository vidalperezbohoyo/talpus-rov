#include <SoftwareSerial.h>

#include "Comms.h" 

const int RX_PIN = 2;
const int TX_PIN = 3;


SoftwareSerial serial (RX_PIN, TX_PIN);

void setup() {
  // Define pin modes for TX and RX
  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);
  pinMode(13, OUTPUT);

  Serial.begin(9600);
  serial.begin(9600);
}

void loop() {
  digitalWrite(13, HIGH);
  while (!Serial.available()) {}; // Wait 
  digitalWrite(13, LOW);
  
  Command command = (Command)Serial.read();

  if (command == Command::TEMPERATURE_HUMIDITY_DATA) {
    receiveTemperatureHumidityData();
  }

}

void receiveTemperatureHumidityData() {
  
  TemperatureHumidityData data;
  int structureSize = (int)sizeof(TemperatureHumidityData);
  while (Serial.available() < structureSize);

    unsigned char* bytes = (unsigned char*)&data;
    for (int i = 0; i < structureSize; i++) {
      bytes[i] = Serial.read();
    }

   data = *(TemperatureHumidityData*)bytes;

   Serial.println("T: ");
   Serial.print(data.temperature);
   Serial.print(" degrees   H: ");
   Serial.print(data.humidity);
}

void receiveBatteryData() {
  
}