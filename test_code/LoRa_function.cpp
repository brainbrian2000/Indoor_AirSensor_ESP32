#include <SPI.h>
#include <LoRa.h>
#include "EEPROM_function.cpp"
#include "Time_func_esp32.cpp"
#include "Wifi_connect.cpp"
class LoRa_function{
    public:
        EEPROM_function eeprom;
        int csPin = 26;           // LoRa radio chip select
        int resetPin = 27;        // LoRa radio reset
        int irqPin = 14;          // change for your board; must be a hardware interrupt pin
        int counter = 0;
        String time_s="";
        String sending="";
        String reading="";
        byte address[4] = {192,168,1,2};
        void start(){
            LoRa.setPins(csPin, resetPin, irqPin);
            for(int i = 0; i <10;i++){
                if(!LoRa.begin(433E6)) {
                    Serial.println("Starting LoRa failed!");
                    // while (1);
                    delay(500);
                }else{
                    Serial.println("Setting done!");
                    break;
                }
            }
        }
        void start(int csPin, int resetPin, int irqPin){
            LoRa.setPins(csPin, resetPin, irqPin);
            for(int i = 0; i <2;i++){
                if(!LoRa.begin(433E6)) {
                    Serial.println("Starting LoRa failed!");
                    // while (1);
                    delay(500);
                }else{
                    Serial.println("Setting done!");
                    break;
                }
            }
        }
        String LoRaMessageWithVerify(String str){
            int len = str.length();
            byte verify=0x00;
            for(int i=0; i<len; i++){
                verify = verify^byte(str[i]);
            }
            // Serial.print("Verify: ");
            // Serial.print(verify,HEX);
            // Serial.println();
            str += char(verify);
            return str;
        }

        void SendEEPROM_WiFiandTime(Time_ESP32 rtc){
            if(!LoRa.begin(433E6)){
                Serial.println("No LoRa.");
                return;
            }
            time_s = rtc.getTimeFormat();
            char ssid[31]={0},userid[32]={0},passwd[32]={0},WiFi_type[2]={0};
            EEPROM_function::WiFi_read(ssid,userid,passwd,WiFi_type);
            sending = "";
            sending +="LoRa sending, now time is "+ time_s+" form WiFi_EEPROM "+ssid;

            String str = LoRaMessageWithVerify(sending);    
            Serial.println("[LoRa] Sending :"+str);
            LoRa.beginPacket();
            LoRa.print(str);
            LoRa.endPacket();
        }
        void ReadLoRaSerial(){
            int packetSize = LoRa.parsePacket();
            if (packetSize) {
                // received a packet
                Serial.println("[Received packet]");

                // read packet
                while (LoRa.available()) {
                // Serial.print((char)LoRa.read());
                    reading+=(char)LoRa.read();
                }
                Serial.println(reading);
                // print RSSI of packet
                Serial.print("[RSSI] ");
                Serial.println(LoRa.packetRssi());
                Serial.println("[LoRa] ReadDone");
                Serial.println();
            }
        }
        void ReadLoRa(){
            int packetSize = LoRa.parsePacket();
            if (packetSize) {
                while (LoRa.available()) {
                    reading+=(char)LoRa.read();
                }
            }
            Serial.println("[LoRa] ReadDone");
        }
        void endLoRa(){
            LoRa.end();
        }
        void SendLongMessage(String message){
            int len = message.length();
            char short_msg[201]={0};
            Serial.println("\n\n[LongMessage]");
            Serial.println(message);
            Serial.println("[end of LongMessage]\n");
            /**
             * @brief Message format: 
             * [S_C0A80102_00]    -> len = 15 -> start of message
             * [M_C0A80102_{HEX}] -> len = 15 -> middle of message
             * [E_C0A80102_{HEX}] -> len = 15 -> end of message 
             * ex:
             * [S_C0A80102_00]~message0~[M_C0A80102_00]
             * [M_C0A80102_01]~message1~[M_C0A80102_01]
             * [M_C0A80102_02]~message1~[M_C0A80102_02]
             * [M_C0A80102_03]~message1~[M_C0A80102_03]
             * [M_C0A80102_04]~message1~[E_C0A80102_04]
             */
            int j =0;
            int message_count = 0;
            char start[16]={0};
            char end[16]={0};
            char SendMsg[240]={0};
            int state =0;
            for(int i=0; i<len;i++){
                if(i%200==0){
                    j=0;
                    if(i!=0){
                        /**
                         * @brief Send part 
                         */
                        // Serial.println(short_msg);
                        if(message_count ==0){
                            sprintf(start,"[S_%02x%02x%02x%02x_%02x]",address[0],address[1],address[2],address[3],message_count);
                            sprintf(end,"[M_%02x%02x%02x%02x_%02x]",address[0],address[1],address[2],address[3],message_count);
                        }else{
                            sprintf(start,"[M_%02x%02x%02x%02x_%02x]",address[0],address[1],address[2],address[3],message_count);
                            sprintf(end,"[M_%02x%02x%02x%02x_%02x]",address[0],address[1],address[2],address[3],message_count);
                        }
                        state = 1;
                    }
                }
                //not using else because consider i%200==0&&i==len-1
                if (i == len-1){
                        if(message_count ==0){
                            sprintf(start,"[S_%02x%02x%02x%02x_%02x]",address[0],address[1],address[2],address[3],message_count);
                            sprintf(end,"[E_%02x%02x%02x%02x_%02x]",address[0],address[1],address[2],address[3],message_count);
                        }else{
                            sprintf(start,"[M_%02x%02x%02x%02x_%02x]",address[0],address[1],address[2],address[3],message_count);
                            sprintf(end,"[E_%02x%02x%02x%02x_%02x]",address[0],address[1],address[2],address[3],message_count);
                        }
                        state = 1;
                }
                if (state == 1){
                    message_count++;  // update later
                    for(int m=0;m<240;m++){
                        SendMsg[m] ='\0'; 
                    }
                    sprintf(SendMsg,"%s%s%s",start,short_msg,end);
                    Serial.println("[LoRa] Sending :");
                    Serial.println(SendMsg);
                    String str = LoRaMessageWithVerify(SendMsg);    
                    LoRa.beginPacket();
                    LoRa.print(str);
                    LoRa.endPacket();
                    delay(100);
                    state =0;
                    for(int m=0;m<201;m++){
                        short_msg[m] = '\0';
                    }
                }
                short_msg[j] = message[i];
                j++;
            }
            Serial.println("[End Transmit]");
            return;
        }




};