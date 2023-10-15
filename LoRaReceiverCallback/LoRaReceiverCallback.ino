#include <SPI.h>
#include <LoRa.h>
#include "esp_task_wdt.h"
#include <EEPROM.h>
#include "OLED096_function.cpp"
#include <pas-co2-ino.hpp>
#include "PASCO2_function.cpp"
#include "BME280_function.cpp"
PASCO2 co2;
BME280_function BME;
OLED096 oled;
const int csPin = 26;          // LoRa radio chip select
const int resetPin = 27;        // LoRa radio reset
const int irqPin = 14;          // change for your board; must be a hardware interrupt pin
void onReceive_while(void *pvParam);
int state_read=0;
int state_valid=0;
int state_oled=0;
String Read_String;
int RSSI = 0;
void setup() {
  Serial.begin(115200);
  // while (!Serial);
    Wire.begin();
  co2.init();
  BME.init();
  EEPROM.begin(512);
  pinMode(5, OUTPUT);
  digitalWrite(5,HIGH);
  LoRa.setPins(csPin, resetPin, irqPin);
  Serial.println("LoRa Receiver Callback");
  oled.init();
  while (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    // while (1);
    delay(1000);
  }

  // Uncomment the next line to disable the default AGC and set LNA gain, values between 1 - 6 are supported
  // LoRa.setGain(6);
  
  // register the receive callback
  LoRa.receive();
  xTaskCreate(freeRTOS_onReceive,"RX_LoRa",2048,NULL,6,NULL);
  delay(100);
  xTaskCreate(freeRTOS_PrintSerial,"PRINT_SERIAL",2048,NULL,7,NULL);
  delay(100);
  xTaskCreate(freeRTOS_OLED,"PRINT_SERIAL",2048,NULL,8,NULL);
  delay(100);
  xTaskCreate(freeRTOS_VerifyXOR,"VERIFY_XOR",1024,NULL,9,NULL);
  delay(100);
  xTaskCreate(freeRTOS_CO2,"CO2",2048,NULL,11,NULL);
  vTaskDelay(10);
  xTaskCreate(freeRTOS_BME,"BME",2048,NULL,12,NULL);
  vTaskDelay(10);
  xTaskCreate(freeRTOS_Serial,"SERIAL",2048,NULL,5,NULL);
  // put the radio into receive mode
}

void loop() {
}
int packetSize=0;

void freeRTOS_OLED(void *pvParam){
  while(true) {
    vTaskDelay(10);
    if(state_oled == 1){
      // Serial.print("loop(OLED) running on core ");
      // Serial.println(xPortGetCoreID());
      char time_cstr[20],wifi_name[25];
      sscanf(Read_String.c_str(),"LoRa sending, now time is %s form WiFi_EEPROM %s",time_cstr,wifi_name);
      oled.print_oled(String(time_cstr)+"\n"+String(wifi_name)+"\nRSSI:"+String(RSSI)+"\nCO2: "+String(co2.co2ppm)+"\nT:"+String(BME.temperature)+"P:"+String(BME.pressure)+"\nRH:"+String(BME.humidity));
      // Serial.println(Read_String);
      // oled.print_oled(Read_String);
      // vTaskDelay(1000);
      vTaskDelay(100);
      state_oled = 0;
    }else{
      // Serial.println("[OLED ] No receiving.");
    }

  }
}
unsigned long int time_onReceive_warning=0;
void onReceive() {
  // if(millis()-time_onReceive_warning>5000){
  //   Serial.println("[RECEIVE] No receive.");
  //   vTaskDelay(10);
  //   time_onReceive_warning = millis();
  // }
  // received a packet
  packetSize = LoRa.parsePacket();
  if (packetSize == 0){
    // esp_task_wdt_reset();
    return;
  }else{
    // vTaskDelay(1);
    // Serial.print("loop(onReceive) running on core ");
    // Serial.println(xPortGetCoreID());
    Read_String = "";
    // read packet
    for (int i = 0; i < packetSize; i++) {
      char temp = 0;
      temp = LoRa.read();
      Read_String += temp;
    }
    RSSI = LoRa.packetRssi();
    state_read = 1;
  }

  // print RSSI of packet
}
void freeRTOS_onReceive(void *pvParam){
  esp_task_wdt_init(5,false);
  while(true){
      onReceive();
  }
}
void freeRTOS_VerifyXOR(void *pvParam){
  while(true){
    vTaskDelay(10);
    if(state_read == 1){
      if(verify_XOR(Read_String.c_str())){
        Serial.println("Verification pass");
        state_read = 0; //have data
        state_valid = 1;
      }else{
        Serial.println("Verification failed");
        state_read = 0; //have data but not valid.
        state_valid = 0;
      }
    }
  }
}

int len;
byte verify;
bool verify_XOR(const char* input_cstr){
  len = strlen(input_cstr);
  verify=0x00;
  // Read_String = "";
  if(len!=0&&len!=1){
    for(int i=0; i<len-1; i++){
      verify = verify^byte(input_cstr[i]);
      // Read_String +=char(input_cstr[i]); 
    }
  }
  Serial.print("Verified: ");
  Serial.print(char(verify));
  Serial.println();
  if(verify == byte(input_cstr[len-1])){
    Read_String[len-1] = '\0';
    return true;
  }
  else{
    if(verify^byte(input_cstr[len-1])==byte(0x00)){
      Read_String[len-1] = '\0';
      return true;
    }else{
      return false;
    }
  }
}


unsigned long millis_time_print = 0;
void freeRTOS_PrintSerial(void *pvParam){
  while(true){
    vTaskDelay(10);
    // if((millis()-millis_time_print)>=1000){
      // millis_time_print = millis();
      if(state_valid !=1){
        // Serial.println("[PRINT] No Receive.");
        // Serial.print("loop(print_while) running on core ");
        // Serial.println(xPortGetCoreID());
        // state_oled = 1;
      }else{
        // OLED();
        // Serial.print("loop(print_while) running on core ");
        // Serial.println(xPortGetCoreID());
        Serial.println(Read_String);
        Serial.print(" with RSSI ");
        Serial.println(RSSI);
        state_valid=0;
        state_oled = 1;
      }
    // }
  }

}
void freeRTOS_CO2(void *pvParm){
  while(true) {
    co2.update_pressure(BME.pressure);
    vTaskDelay(co2.MEASURE_INTERVAL*1000);
  }
}

void freeRTOS_BME(void *pvParm){
  while(true) {
    vTaskDelay(5000);
    BME.getData();
  }
}
void freeRTOS_Serial(void * pvParm){
  while(true) {
    vTaskDelay(5000);
    Serial.print("CO2: ");
    Serial.println(co2.getCO2());
    Serial.println("Temp: "+String(BME.temperature)+" Pressure: "+String(BME.pressure)+" Humidity: "+String(BME.humidity));
    state_oled = 1;
  }
}