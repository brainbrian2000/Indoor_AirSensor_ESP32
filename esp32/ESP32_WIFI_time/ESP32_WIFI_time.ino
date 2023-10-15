#include <WiFi.h>
#include "time.h"
#include <esp_bt.h>
#include <esp_wifi.h>
#include <esp_sleep.h>
#include <ESP32Time.h>
ESP32Time rtc(3600*8);
ESP32Time rtc_UTC(0);
const char* ssid = "SCplusNTU";
const char* password = "smartcampus206";

// date and time parameters
const char* ntpServer = "pool.ntp.org";
long gmtOffset_sec = 0;  // time zone GMT
// long gmtOffset_sec = 8 * 60 * 60;  // time zone GMT
const int daylightOffset_sec = 0;  //daylight saving time

// deep sleep mode parameters
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10       /* Time ESP32 will go to sleep (in seconds) */


bool workstate = 0;
unsigned long int time_sleep=0;
unsigned long int current_time=0;
void setup() {
  Serial.begin(115200);
  delay(1000);

  // connect to wifi
  WiFi.begin(ssid, password);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());


  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);//update with UTC time and print with LST time by ESP32Time
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  delay(10);
  //deep sleep mode 
  Serial.println("DEEP SLEEP ENABLED");
  delay(10);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  // Pay attention to this command, with this wake up source
  // your device can't go to hibernate mode
  // but only deep sleep
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");
  delay(10);
  // esp_deep_sleep_start();
  
  esp_light_sleep_start();
}
void loop() {
  // delay(10000);
  current_time = millis();
  if(current_time-time_sleep>1000){
    workstate = 0;
    esp_light_sleep_start();
  }
  if(workstate==0){
    time_sleep = millis();
    Serial.println("ESP32 Wake up for every 10s" );
    printLocalTime();
    Serial.println("ESP32 Sleep");
    workstate = 1;
  }
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
  Serial.println(rtc_UTC.getTime());
  Serial.print("RTC LST Time:");
  Serial.println(rtc.getTime());
}
