#ifndef BME280_FUNCTION_CPP
#define BME280_FUNCTION_CPP
/**
 * @brief  This function can using in not only BME280 also can using in BMP280
 *  Using BME280I2C.h library
 *  Using I2C
 */
#include <BME280I2C.h>
#include <Wire.h>



class BME280_function{
    public:
    BME280I2C bme;
    float temperature=0,humidity=0,pressure=0;
    BME280_function(){};
    ~BME280_function(){};
    void init(){
            bme.begin();
            vTaskDelay(100);
            // if(!bme.begin()) return;
            // {
            //     Serial.println("Could not find BME280 sensor!");
            //     delay(1000);
            // }
            switch(bme.chipModel())
            {
                case BME280::ChipModel_BME280:
                Serial.println("Found BME280 sensor! Success.");
                break;
                case BME280::ChipModel_BMP280:
                Serial.println("Found BMP280 sensor! No Humidity available.");
                break;
                default:
                Serial.println("Found UNKNOWN sensor! Error!");
            }
            return;
    };
    void getData(){
        BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
        BME280::PresUnit presUnit(BME280::PresUnit_hPa);
        bme.read(pressure, temperature, humidity, tempUnit, presUnit);
    }
    float getTemperature(){
        temperature = bme.temp();
        return temperature;
    }
    float getHumidity(){
        humidity = bme.hum();
        return humidity;
    }
    float getPressure(){
        pressure = bme.pres();
        return pressure;
    }

};

#endif