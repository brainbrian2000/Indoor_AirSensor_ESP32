#include <SPI.h>
#include <LoRa.h>
#include <EEPROM.h>
#include <WiFi.h>
#include "EEPROM_function.cpp"
#include "Time_func_esp32.cpp"
#include "Wifi_connect.cpp"
const int csPin = 26;          // LoRa radio chip select
const int resetPin = 27;        // LoRa radio reset
const int irqPin = 14;          // change for your board; must be a hardware interrupt pin
int counter = 0;
Time_ESP32 rtc(8);
Time_ESP32 rtc_UTC(0);
EEPROM_function eeprom;
const char* ssid = "Riceball_Fan";
const char* password = "brainbrian2000";
struct tm timeinfo;
WiFi_connection wifi_connection;
void printLocalTime();

void setup() {


  Serial.begin(115200);
  while (!Serial);
  EEPROM.begin(512);
  // wifi_connection.changeWPA2(ssid,password);
  wifi_connection.turn_on_WiFi(0);
  wifi_connection.turn_on_WiFi(1);
  rtc.update_onlinetime();
  printLocalTime();
  
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

String time_s;
void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);
  time_s = rtc.getTimeFormat();
  // send packet
  LoRa.beginPacket();
  LoRa.print("hello, now time is "+time_s);
  Serial.println("hello, now time is "+time_s);

  LoRa.print(".");
  LoRa.endPacket();

  counter++;
  if(counter %5==0){
    counter = 0;
    rtc.update_onlinetime();
  }  
  delay(5000);
}
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.print("DATE :  ");
  Serial.println(&timeinfo, "%F");
  Serial.print("TIME :  ");
  Serial.println(&timeinfo, "%R");
  Serial.print("RTC UTC Time:");
  Serial.println(rtc_UTC.getTimeFormat());
  Serial.print("RTC LST Time:");
  Serial.println(rtc.getTimeFormat());
  // RFID_module
  // UpdateKey
}
