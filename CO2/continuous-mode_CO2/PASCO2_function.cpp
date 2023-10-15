#ifndef PASCO2_FUNCTION_CPP
#define PASCO2_FUNCTION_CPP
#include <pas-co2-ino.hpp>
#include <Wire.h>

class PASCO2{
    public:
        PASCO2Ino cotwo;
        int16_t co2ppm;
        Error_t err;
        int MEASURE_INTERVAL=5;
        int pressure = 1013;
        PASCO2(){};
        ~PASCO2(){};
        void init(){
            // Wire.begin();
            /* Initialize the sensor */
            err = cotwo.begin();
            if(XENSIV_PASCO2_OK != err)
            {
            Serial.print("initialization error: ");
            Serial.println(err);
            }

            /* We can set the reference pressure before starting 
            * the measure 
            */
            err = cotwo.setPressRef(pressure);
            if(XENSIV_PASCO2_OK != err)
            {
            Serial.print("pressure reference error: ");
            Serial.println(err);
            }

            /*
            * Configure the sensor to measureme periodically 
            * every 10 seconds
            */
            err = cotwo.startMeasure(MEASURE_INTERVAL);
            if(XENSIV_PASCO2_OK != err)
            {
            Serial.print("start measure error: ");
            Serial.println(err);
            }
        };
        void update_pressure(int new_pressure){
            pressure = new_pressure;
        }
        int getCO2(bool UpdatePressure=true){
            // vTaskDelay(MEASURE_INTERVAL*5);
            err = cotwo.getCO2(co2ppm);
            if(XENSIV_PASCO2_OK != err){
            /* Retry in case of timing synch mismatch */
                if(XENSIV_PASCO2_ERR_COMM == err){
                    delay(600);
                    err = cotwo.getCO2(co2ppm);
                    if(XENSIV_PASCO2_OK != err)          
                    {
                    Serial.print("get co2 error: ");
                    Serial.println(err);
                    }
                }
            }
            err = cotwo.setPressRef(pressure);
            if(XENSIV_PASCO2_OK != err){
                Serial.print("pressure reference error: ");
                Serial.println(err);
            }
            return co2ppm;
        }
        void close(){
            cotwo.stopMeasure();
        }
};




#endif