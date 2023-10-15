
#include "SIM7000.h"
#define ctrlSIMCOM  5
#define autoset 0
SIM7000 NBIOT;





void setup() {
  Serial.begin(115200);

  NBIOT.begin(9600);
  NBIOT.setDebuger(Serial);
  NBIOT.init(ctrlSIMCOM);


}

void loop() {
  NBIOT.ON(false);
  NBIOT.AT_Test();
  NBIOT.getGPS();
  NBIOT.AT_Test();

  // TCP connexion

  bool connected_flag = false;
  if (connected_flag==false){
    connected_flag = NBIOT.openNetwork("scplus-iot.ipcs.ntu.edu.tw", 1883,autoset);
    }
    Serial.print("Connected_flag : ");
    Serial.println(connected_flag);
    delay(1000);

  if(connected_flag != 0){ // value proof of connexion 

    Serial.println("Connected");

    NBIOT.AT_CMD("AT+CIPSEND", true);
    NBIOT.AT_print("hello");
    NBIOT.AT_end();
    
    delay(2000);
}

}