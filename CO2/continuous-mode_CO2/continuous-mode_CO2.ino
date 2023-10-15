#include <Arduino.h>
#include <pas-co2-ino.hpp>
#include "PASCO2_function.cpp"
#include "BME280_function.cpp"


PASCO2 co2;
BME280_function BME;



void setup()
{
  Wire.begin();
  co2.init();
  BME.init();
  delay(1000);
            
  Serial.begin(115200);

  xTaskCreate(freeRTOS_CO2,"CO2",2048,NULL,11,NULL);
  vTaskDelay(10);
  xTaskCreate(freeRTOS_BME,"BME",2048,NULL,12,NULL);
  vTaskDelay(10);
  xTaskCreate(freeRTOS_Serial,"SERIAL",2048,NULL,9,NULL);

}

void loop()
{
  // delay(co2.MEASURE_INTERVAL*1000);
  
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
  }
}