
#include "SIM7000.h"
#define ctrlSIMCOM  5
#define autoset 0
SIM7000 NBIOT;


void setup() {
  
  Serial.begin(9600);
  NBIOT.begin(9600);
  NBIOT.AT_Test();
  Serial.print(NBIOT.AT_Test());
  NBIOT.setDebuger(Serial);
  NBIOT.init(ctrlSIMCOM);


}

void loop() {

  NBIOT.ON(false);
  NBIOT.AT_Test();
  NBIOT.getGPS();
  Serial.println("[NBIOT] GPS TimeTag " + NBIOT.GPSTimeTag);
  NBIOT.AT_Test();


  // TCP connexion


   NBIOT.openNetwork("scplus-iot.ipcs.ntu.edu.tw", 1883,autoset);
   
  delay(1000);
  Serial.println("Connected");
  NBIOT.AT_CMD("AT+SMCONF=\"URL\",\"scplus-iot.ipcs.ntu.edu.tw\",\"1883\"",true);
  NBIOT.AT_CMD("AT+SMCONF=\"USERNAME\",\"riceball\"",true);
  NBIOT.AT_CMD("AT+SMCONF=\"TOPIC\",\"NTU/SmartCampus/NTU4AQ_riceball_test\"",true);
  NBIOT.AT_CMD("AT+SMCONF=\"KEEPTIME\",\"60\"",true);
  NBIOT.AT_CMD("AT+SMCONF=\"QOS\",\"1\"",true);
  NBIOT.AT_CMD("AT+SMCONF=\"CLEANSS\",\"1\"",true);
  NBIOT.AT_CMD("AT+SMCONF=\"MESSAGE\",\"nbiot-test-20220714\"",true);

  NBIOT.AT_CMD("AT+SMCONN",true);
  NBIOT.AT_CMD("AT+SMPUB=\"NTU/SmartCampus/NTU4AQ_riceball_test\",\"10\",\"1\",\"1\"",true);

  NBIOT.AT_print("hello you");
  NBIOT.AT_end();
  delay(3000);


  NBIOT.closeNetwork();
  NBIOT.OFF();

}
