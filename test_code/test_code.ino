#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include "time.h"
#include <esp_bt.h>
#include <esp_wifi.h>
#include <esp_sleep.h>
#include <ESP32Time.h>
#include "Time_func_esp32.cpp"
#include "Wifi_connect.cpp"
#include "RFID_function.cpp"
#include "EEPROM_function.cpp"
#include "LoRa_function.cpp"
#include "OLED096_function.cpp"
#include "MQTT_function.cpp"
// #include <EEPROM.h>
// ESP32Time rtc(3600*8);
// ESP32Time rtc_UTC(0);
Time_ESP32 rtc(8);
Time_ESP32 rtc_UTC(0);
RFID_module rfid;
EEPROM_function eeprom;
LoRa_function lora;
OLED096 lcd;
MQTT_function mqtt;
// const char* ssid = "SCplusNTU";
// const char* password = "smartcampus206";

// date and time parameters
const char* ntpServer = "pool.ntp.org";
long gmtOffset_sec = 0;  // time zone GMT
// long gmtOffset_sec = 8 * 60 * 60;  // time zone GMT
const int daylightOffset_sec = 0;  //daylight saving time

// deep sleep mode parameters
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  9       /* Time ESP32 will go to sleep (in seconds) */
struct tm timeinfo;
WiFi_connection wifi_connection;
bool workstate = 0;
unsigned long int time_sleep=0;
unsigned long int current_time=0;
char ssid[31]={0},userid[32]={0},passwd[32]={0},WiFitype[2]={0};
void setup() {
  eeprom.Init();
  SPI.begin();
  Serial.begin(115200);
  delay(1000);
  // pinMode(26, OUTPUT);
  // digitalWrite(26,HIGH);


  // strcpy(wifi_connection.ssid_wpa2,"Riceball_Fan");
  // strcpy(wifi_connection.password_wpa2,"brainbrian2000");
  // wifi_connection.turn_on_WiFi(0);
  // wifi_connection.turn_on_WiFi(1,1);
  setWiFiByEEPROM();


  // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);//update with UTC time and print with LST time by ESP32Time
  printLocalTime();
  rtc.update_onlinetime();
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  wifi_connection.turn_off_WiFi();

  delay(10);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  // Pay attention to this command, with this wake up source
  // your device can't go to hibernate mode
  // but only deep sleep
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP+1) +
  " Seconds");
  //deep sleep mode 
  delay(10);
  Serial.println("DEEP SLEEP ENABLED");
  delay(10);
  lcd.init();
  // rfid.start();
  Wire.end();
  SPI.end();
  Serial.flush();


  EEPROM_function::WiFi_read(ssid,userid,passwd,WiFitype);
  esp_deep_sleep_start();
  // esp_light_sleep_start();
}
int WiFi_time_counter = 0;

void loop() {
  // delay(10000);
  current_time = millis();
  if(current_time-time_sleep>1000){
    workstate = 0;
    lora.endLoRa();
    SPI.end();
    rfid.powerdown();
    // while(timeinfo.tm_sec%5 !=1){
    delay(100);
    // }
  esp_deep_sleep_start();
    // esp_light_sleep_start();
  }

  if(workstate==0){
    SPI.begin();
    rfid.start();
    lora.start();
    time_sleep = millis();
    Serial.println("ESP32 Wake up for every 5s, checkWiFi Time for every 20 times." );
    if(WiFi_time_counter >=20){
      setWiFiByEEPROM();
      EEPROM_function::WiFi_read(ssid,userid,passwd,WiFitype);
      rtc.update_onlinetime();
      WiFi_time_counter = 0;
    }    
    WiFi_time_counter++;
    // rfid.mfrc522.PCD_SoftPowerUp();
    // rfid.mfrc522.PCD_AntennaOn();
    rfid.powerup();
    // rfid.mfrc522.PCD_DumpVersionToSerial();  //
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
      bool rfid_wifi_check = rfid.ReadWiFi_RFID(ssid,userid,passwd,WiFitype);


      if(rfid_wifi_check){
        Serial.println("Print By Char arr passed to main function.");
        Serial.println(rfid.ReadStrFromSector(3));
        eeprom.WiFi_write(ssid,userid,passwd,WiFitype);
        eeprom.WiFi_read();
        setWiFiByEEPROM();
        WiFi_time_counter=0;
      }      
      rfid.halt();






    }else{
      // rfid.halt();
      // Serial.println(rfid.mfrc522.PICC_ReadCardSerial());
      eeprom.WiFi_read();
      Serial.println("No RFID card");
      delay(10);
    }
    // rfid.mfrc522.PCD_AntennaOff();
    // rfid.mfrc522.PCD_SoftPowerDown();
    rfid.powerdown();

    rtc.update();
    Serial.println(rtc.getTimeFormat());

    lcd.print_oled(rtc.getTimeFormat()+"\n"+ssid);
    Serial.println(millis());
    lora.SendEEPROM_WiFiandTime(rtc);
    // int jsonStringLen[4] = {50,200,350,500};
    // for(int i = 0; i <4 ;i++){
    //   lora.SendLongMessage(generateJsonString(jsonStringLen[i]));
    // }
    Serial.println(millis());
    Serial.println("ESP32 Sleep");
    delay(100);
  }
}


String generateJsonString(int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int charsetSize = sizeof(charset) - 1;
    char json[length + 1]; // +1 for null-terminator
    char output[length + 20];
    for (int i = 0; i < length; i++) {
        json[i] = charset[rand() % charsetSize];
    }
    json[length] = '\0';

    // printf("{\"key\":\"%s\"};\n", json);
    sprintf(output,"{\"key\":\"%s\"};", json);
    return String(output);
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

  }
  else if(WiFi_type[0] == '2'){
      strcpy(wifi_connection.peap_ssid_wpa2,ssid);
      strcpy(wifi_connection.peap_username_wpa2,userid);
      strcpy(wifi_connection.peap_password_wpa2,passwd);
      wifi_connection.turn_on_WiFi(2,1);
      rtc.update_onlinetime();
      printLocalTime();
  }
  mqtt.setup_mqtt();
  mqtt.publish(rtc.getTimeFormat()+" form "+String(ssid));
  Serial.println(rtc.getTimeFormat()+" form "+String(ssid));
  delay(1000);
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
