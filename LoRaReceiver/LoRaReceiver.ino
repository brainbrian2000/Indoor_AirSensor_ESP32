#include <SPI.h>
#include <LoRa.h>
#include <EEPROM.h>

const int csPin = 26;          // LoRa radio chip select
const int resetPin = 27;        // LoRa radio reset
const int irqPin = 14;          // change for your board; must be a hardware interrupt pin

void setup() {
  Serial.begin(115200);
  while (!Serial);
  pinMode(5, OUTPUT);
  digitalWrite(5,HIGH);
  Serial.println("LoRa Receiver");
  EEPROM.begin(512);
  LoRa.setPins(csPin, resetPin, irqPin);
  while (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    // while (1);
    delay(500);
  }
  Serial.println("Setting done!");
}

void loop() {
  // try to parse packet
  // Serial.println("LoRa Testing");
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}
