#include <Arduino.h>
#include <EEPROM.h>
// #include "SoftwareSerial.h"
#include "esp_task_wdt.h"
#include "SIM7000_function.cpp"
SIM7000_FUNCTION SIM7000;
// SoftwareSerial SIM7000(16,17);
void setup() {
    Serial.begin(115200);  // 初始化串口监视器
    // while(!Serial.){
    EEPROM.begin(512);
    // }
    Serial2.begin(9600);   // 初始化 Serial2
    // .begin(9600);   // 初始化 Serial2
    esp_task_wdt_init(5,false);  //avoid watchdog restart the MCU
    
    Serial.println("Setting Start");
    xTaskCreate(SIM7000.freeRTOS_SerialRead,"SERIAL_READ",4096,NULL,8,NULL);
    delay(100);
    xTaskCreate(SIM7000.freeRTOS_MessagePrint,"SERIAL_READ",4096,NULL,7,NULL);
    delay(100);
    xTaskCreate(Serial_Read_Print,"Manual",1024,NULL,5,NULL);
    SIM7000.AT_test();
    delay(1000);
    SIM7000.GPS();
    delay(1000);
    SIM7000.SetLTEConnection();
    SIM7000.SendCommand(F("AT+CIPPING=\"8.8.8.8\""));
    // Serial.println("Setting done");
}

void loop() {
    // else{
        // Serial.println("Serial is not available");
// }

    // Serial2.write("AT\r\n");        // 将字符发送到 Serial2
    // delay(1000);
    // while (Serial2.available()) {
    //     char receivedChar = Serial2.read(); // 从 Serial2 读取接收到的字符
    //     Serial.print(receivedChar);         // 将接收到的字符发送到串口监视器
    // }
    // Serial.println();
}
void Serial_Read_Print(void *pvParm){
    String Message = "";
    int state_message = 0;
    while(true){
        vTaskDelay(10);
        if(Serial.available()){
            Message += char(Serial.read());
            state_message =1;     //Reading State
            if(!Serial.available()){
                state_message = 2 ; //Read Done
            }
        }
        if(state_message == 2){
            Serial.println("[Sending]"+Message);
            Serial2.write(Message.c_str());
            state_message = 0;
            Message="";
        }

    }
    // while (Serial.available()) {
    //     char inputChar = Serial.read();  // 从串口监视器读取一个字符
    //     Serial.print(byte(inputChar),HEX);
    //     Serial.print(' ');
    // } 
}