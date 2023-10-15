#include <SPI.h>
#include <LoRa.h>
#include <EEPROM.h>
const int csPin = 26;          // LoRa radio chip select
const int resetPin = 27;        // LoRa radio reset
const int irqPin = 14;          // change for your board; must be a hardware interrupt pin
int counter = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  EEPROM.begin(512);
  pinMode(5, OUTPUT);
  digitalWrite(5,HIGH);
  Serial.println("LoRa Sender");

  LoRa.setPins(csPin, resetPin, irqPin);
  while (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    // while (1);
    delay(500);
  }
  Serial.println("Setting done!");
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(5000);
}
