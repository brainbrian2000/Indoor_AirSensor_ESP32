#include <Arduino.h>
#line 1 "E:\\.NTU\\IPCS\\airsensor\\test_code\\test_code.ino"
#include <WiFi.h>
#include <SPI.h>
#include "time.h"
#include <esp_bt.h>
#include <esp_wifi.h>
#include <esp_sleep.h>
#include <ESP32Time.h>
#include "Time_func_esp32.cpp"
#include "Wifi_connect.cpp"
#include "RFID_function.cpp"
#include "EEPROM_function.cpp"
// #include <EEPROM.h>
// ESP32Time rtc(3600*8);
// ESP32Time rtc_UTC(0);
Time_ESP32 rtc(8);
Time_ESP32 rtc_UTC(0);
RFID_module rfid;
EEPROM_function eeprom;
const char* ssid = "SCplusNTU";
const char* password = "smartcampus206";

// date and time parameters
const char* ntpServer = "pool.ntp.org";
long gmtOffset_sec = 0;  // time zone GMT
// long gmtOffset_sec = 8 * 60 * 60;  // time zone GMT
const int daylightOffset_sec = 0;  //daylight saving time

// deep sleep mode parameters
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  4       /* Time ESP32 will go to sleep (in seconds) */
struct tm timeinfo;
WiFi_connection wifi_connection;
bool workstate = 0;
unsigned long int time_sleep=0;
unsigned long int current_time=0;
#line 36 "E:\\.NTU\\IPCS\\airsensor\\test_code\\test_code.ino"
void setup();
#line 77 "E:\\.NTU\\IPCS\\airsensor\\test_code\\test_code.ino"
void loop();
#line 142 "E:\\.NTU\\IPCS\\airsensor\\test_code\\test_code.ino"
void setWiFiByEEPROM();
#line 170 "E:\\.NTU\\IPCS\\airsensor\\test_code\\test_code.ino"
void printLocalTime();
#line 36 "E:\\.NTU\\IPCS\\airsensor\\test_code\\test_code.ino"
void setup() {
  eeprom.Init();
  Serial.begin(115200);
  delay(1000);



  // strcpy(wifi_connection.ssid_wpa2,"Riceball_Fan");
  // strcpy(wifi_connection.password_wpa2,"brainbrian2000");
  wifi_connection.turn_on_WiFi(0);
  // wifi_connection.turn_on_WiFi(1,1);
  setWiFiByEEPROM();


  // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);//update with UTC time and print with LST time by ESP32Time
  printLocalTime();
  rtc.update_onlinetime();
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  wifi_connection.turn_off_WiFi();

  delay(10);
  //deep sleep mode 
  Serial.println("DEEP SLEEP ENABLED");
  delay(10);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  // Pay attention to this command, with this wake up source
  // your device can't go to hibernate mode
  // but only deep sleep
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP+1) +
  " Seconds");
  delay(10);
  // esp_deep_sleep_start();
  // rfid.start();
  rfid.start();
  // while(timeinfo.tm_sec%5 !=1){
  //   delay(100);
  // }
  esp_light_sleep_start();
}
void loop() {
  // delay(10000);
  current_time = millis();
  if(current_time-time_sleep>1000){
    workstate = 0;
    // while(timeinfo.tm_sec%5 !=1){
    //   delay(100);
    // }
    esp_light_sleep_start();
  }
  if(workstate==0){
    time_sleep = millis();
    Serial.println("ESP32 Wake up for every 5s" );
    rfid.mfrc522.PCD_AntennaOn();
    workstate = 1;
    if(rfid.scan()){
      // strcpy(wifi_connection.peap_password_wpa2,"TeStInGWiFiPASSwd@ipcs206");
      // rfid.writeWiFi_RFID(wifi_connection.peap_ssid_wpa2,wifi_connection.peap_username_wpa2,wifi_connection.peap_password_wpa2);
      // rfid.writeWiFi_RFID(wifi_connection.ssid_wpa2,wifi_connection.password_wpa2);
      // rfid.writeWiFi_RFID("Riceball_Fan","brainbrian2000");
      // rfid.WriteStrToSector(3,"This is writing Test,\nI hope It can print.");
      for (byte i = 0; i < rfid.mfrc522.uid.size; i++) {
        Serial.print(rfid.mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.mfrc522.uid.uidByte[i], HEX);
      }
      Serial.print("\n");
      // rfid.sectorDump(1,true);
      // rfid.sectorDump(2,true);
      rfid.ReadWiFi_RFID();
      char ssid[31]={0},userid[32]={0},passwd[32]={0},WiFitype[2]={0};
      rfid.ReadWiFi_RFID(ssid,userid,passwd,WiFitype);
      Serial.println("Print By Char arr passed to main function.");
      Serial.println(ssid);
      Serial.println(userid);
      Serial.println(passwd);
      Serial.println(WiFitype);
      Serial.println(rfid.ReadStrFromSector(3));
      eeprom.WiFi_write(ssid,userid,passwd,WiFitype);
      eeprom.WiFi_read();
      rfid.halt();
      setWiFiByEEPROM();





    }else{
      // rfid.halt();
      // Serial.println(rfid.mfrc522.PICC_ReadCardSerial());
      eeprom.WiFi_read();
      Serial.println("No RFID card");
      delay(10);
    }

    printLocalTime();
    rtc.update();
    Serial.println(rtc.getTimeFormat());

    rfid.mfrc522.PCD_AntennaOff();
    Serial.println("ESP32 Sleep");
    delay(100);
  }


}
void setWiFiByEEPROM(){
  // need to write the condition of update ->read by rfid card?
  char ssid[31]={0},userid[32]={0},passwd[32]={0},WiFi_type[2]={0};
  EEPROM_function::WiFi_read(ssid,userid,passwd,WiFi_type);
  if(WiFi_type[0] == '1'){
    strcpy(wifi_connection.ssid_wpa2,ssid);
    strcpy(wifi_connection.password_wpa2,passwd);
    wifi_connection.turn_on_WiFi(1,1);
    rtc.update_onlinetime();
    printLocalTime();
    // wifi_connection.turn_off_WiFi();
  }
  else if(WiFi_type[0] == '2'){
    strcpy(wifi_connection.peap_ssid_wpa2,ssid);
    strcpy(wifi_connection.peap_username_wpa2,userid);
    strcpy(wifi_connection.peap_password_wpa2,passwd);
    wifi_connection.turn_on_WiFi(2,1);
    rtc.update_onlinetime();
    printLocalTime();
    // wifi_connection.turn_off_WiFi();
  }
  wifi_connection.PingTest();
  wifi_connection.turn_off_WiFi();
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

