#include <stdlib.h>
#include <EEPROM.h>
#define EEPROM_SIZE 512
/**
 * @brief MEM allocate as RFID card ->Storage about WiFi ssid, username and password.
 * Cost about 48*2=96 bytes
 * Total 512 byte by set->Max of it is 32KB
 * ESP32 do not have real EEPROAM, but can use flash to achieve the function of EEPROAM.
 * ->EEPROM.commit()->to storage the data into EEPROM.
 * 
 */
// using namespace EEPROM;
class EEPROM_function{
    public:
        EEPROM_function(){};
        ~EEPROM_function(){};
        static void Init(){
            EEPROM.begin(EEPROM_SIZE);
        };
        /**
         * @brief Using Address begin from 16 
         * 
         * @param ssid @[16:46]        len:31 
         * @param userid @[48:79]      len:32
         * @param password @[80:111]   len:32
         * @param WiFi_type @[47]      len:1
         */
        static void WiFi_write(const char ssid[31], const char userid[32], const char passwd[32],const char WiFi_type[1]){
            for(int i=0;i<31;i++){
                EEPROM.writeChar(i+16,ssid[i]);
                EEPROM.writeChar(i+48,userid[i]);
                EEPROM.writeChar(i+80,passwd[i]);
            }
            EEPROM.writeChar(79,userid[31]);
            EEPROM.writeChar(111,passwd[31]);
            EEPROM.writeChar(47,WiFi_type[0]);
            EEPROM.commit();
            return;
        };
        static void WiFi_read(char* ssid, char* userid, char* passwd, char* WiFi_type){
            for(int i=0;i<31;i++){
                ssid[i] = EEPROM.readChar(i+16);
                userid[i] = EEPROM.readChar(i+48);
                passwd[i] = EEPROM.readChar(i+80);
            }
            ssid[30] = EEPROM.readChar(46);
            userid[31] = EEPROM.readChar(79);
            passwd[31] = EEPROM.readChar(111);
            WiFi_type[0] = EEPROM.readChar(47);
            // EEPROM.commit();
            Serial.println("[DEBUG] EEPROM READ:");
            Serial.println(ssid);
            Serial.println(userid);
            Serial.println(passwd);
            Serial.println(WiFi_type);
            return;
        };
        static void WiFi_read(){
            char ssid[31]={0},userid[32]={0},passwd[32]={0},WiFi_type[2]={0};
            for(int i=0;i<31;i++){
                ssid[i] = EEPROM.readChar(i+16);
                userid[i] = EEPROM.readChar(i+48);
                passwd[i] = EEPROM.readChar(i+80);
            }
            ssid[30] = EEPROM.readChar(46);
            userid[31] = EEPROM.readChar(79);
            passwd[31] = EEPROM.readChar(111);
            WiFi_type[0] = EEPROM.readChar(47);
            // EEPROM.commit();
            Serial.println("[DEBUG] EEPROM READ:");
            Serial.println(ssid);
            Serial.println(userid);
            Serial.println(passwd);
            Serial.println(WiFi_type);
            Serial.println("[DEBUG] EEPROM READ DONE");
            return;
        };

};