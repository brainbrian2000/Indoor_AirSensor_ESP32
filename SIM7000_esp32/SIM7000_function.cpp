#ifndef SIM7000_FUNCTION_CPP
#define SIM7000_FUNCTION_CPP
#include <Arduino.h>

static String Message;
static int state_message=0;
static int pause_message=0; //0 for continuous,1 for paused
class SIM7000_FUNCTION{
    /** Function
     * SIM7000 using hardware serial
    */
    // HardwareSerial SIMSerial;

    public:
        // HardwareSerial Serial2;
        SIM7000_FUNCTION(){
            // SIMSerial = Serial2;
            Message = "";
            state_message = 0;
        }
        // static void init_SIM7000()
        static void SendCommand(String command){
            // Serial.println("[SENDING]");
            // Serial.println(command);
            Serial2.write(command.c_str());
            Serial2.write("\r\n");
        }

        static void SendCommand(const char *command){
            // Serial.println("[SENDING]");
            // Serial.println(command);
            Serial2.write(command);
            Serial2.write("\r\n");
        }

        static void AT_test(){
            bool TestState = false;
            bool HoldState = false;
            // while(true){
                while(!TestState && !HoldState){
                    vTaskDelay(1000);
                    Serial2.write("AT\r\n");
                    HoldState=true;
                    pause_message = 1;
                    while(state_message != 2){  // Wait for Read Done
                        vTaskDelay(10);
                    }
                    char getMessage[30];
                    sscanf(Message.c_str(),"AT\n%s\n",getMessage);
                    if(!strcmp(getMessage,"OK")){
                        Serial.println("AT is OK");
                    }
                    Serial.println("[DEBUG]");
                    Serial.println(getMessage);
                    Serial.println("[DEBUG END]");
                    pause_message = 0;
                }
                // return;
                // vTaskDelay(100);
            // }
        }

        static void freeRTOS_MessagePrint(void *pvParm){
            while(true){
                if(state_message ==2 && pause_message==0){
                    Serial.println(Message);
                    state_message=0;
                }
                delay(50);
            }
        }        
        static void MessageClear(){
            if(state_message==2 && pause_message==0){
                state_message=0; // into Clear mode
                Message = "";
            }
        }
        /**
         * @brief 
         * This function is for monitoring the Serial message from SIM7000 module and print it.
         * Using FreeRTOS to makesure it work in While and not blocking other function and method.
         * @param pvParm 
         */
        static void freeRTOS_SerialRead(void *pvParm){
            while(true){
                vTaskDelay(10);
                if(Serial2.available() && state_message!=2){
                    Message += char(Serial2.read());
                    state_message =1;     //Reading State
                    if(!Serial2.available()){
                        state_message =2; //Read Done State
                    }
                
                }

                if (!Message.equals("") && state_message == 0){
                    Message = "";
                }
            }
        }
        static void GPS(){
            Serial.println("[GPS SETTING]");
            SendCommand(F("AT+CGNSPWR=1"));
            vTaskDelay(500);
            SendCommand(F("AT+CGNSHOT"));
            vTaskDelay(500);
            // SendCommand(F("AT+CGNSURC=3"));
            GPSGetInfo();
        }
        static void GPSGetInfo(){
            int repeat_times = 0;
            int fail_repeat_times = 0;
            bool GetGPSInfo = false;
            while(!GetGPSInfo){
                vTaskDelay(1000);
                // SendCommand(F("AT+CGNSTST=1,1"));
                // vTaskDelay(3000);
                SendCommand(F("AT+CGNSINF"));
                pause_message = 1;
                while(state_message != 2){  // Wait for Read Done
                    vTaskDelay(10);
                }
                // unsigned long int DateTime;
                char DateTime[19],lon[11],lat[11],alt[11],speed[10];
                char SubMessage[120];
                int pwr=0,state=0;
                // float lon,lat,alt,speed;
                sscanf(Message.c_str(),"AT+CGNSINF\n+CGNSINF: %1d,%1d,%s",&pwr,&state,SubMessage);
                // sscanf(Message.c_str(),"AT+CGNSINF\n+CGNSINF: %d,%d,%f,%f,%f,%f,%f,%s",&pwr,&state,&DateTime,&lon,&lat,&alt,&speed,Trash);
                //+CGNSINF: 1,1,20230812085046.000,24.987569,121.428269,39.300,0.00,187.1,1,,1.5,1.8,0.9,,23,3,3,,29,,
                if(state == 1){
                    Serial.println("[GNSS is OK]");
                    Serial.println(SubMessage);
                    char * token;
                    char * saveptr;
                    token = strtok_r(SubMessage,",",&saveptr);
                    strcpy(DateTime,token);
                    token = strtok_r(NULL,",",&saveptr);
                    strcpy(lon,token);
                    token = strtok_r(NULL,",",&saveptr);
                    strcpy(lat,token);
                    token = strtok_r(NULL,",",&saveptr);
                    strcpy(alt,token);
                    token = strtok_r(NULL,",",&saveptr);
                    strcpy(speed,token);
                    Serial.println(DateTime);
                    Serial.println("LON   "+String(lon));
                    Serial.println("LAT   "+String(lat));
                    Serial.println("ALT   "+String(alt));
                    Serial.println("SPEED "+String(speed));
                    if(repeat_times<=4){
                        repeat_times++;
                    }else{
                        GetGPSInfo = true;
                    }
                }else{
                    Serial.println("[GNSS not ok]");
                    if(fail_repeat_times<=5){
                        fail_repeat_times++;
                    }else{
                        GetGPSInfo = true;
                        // break;
                    }
                    // SendCommand(F("AT+CGNSTST"));
                    // while(state_message != 2){  // Wait for Read Done
                    //     vTaskDelay(10);
                    // }
                    // Serial.println(Message);
                }
                pause_message = 0;

            }
            if(GetGPSInfo){
                // SendCommand(F("AT+CGNSPWR=0"));
            }
        }

        static bool SetLTEConnection(){
            SendCommand("AT+CIMI");
            vTaskDelay(300);
            pause_message = 1;
            String _apn="";
            for(int i = 10; i <15;i++){
                _apn+=char(Message.c_str()[i]);
                // Serial.println(Message.c_str()[i]);
                // Serial.println(i);
            }
            vTaskDelay(300);
            pause_message = 0;
            Serial.println("[DEBUG]");
            Serial.println(Message);
            Serial.println(_apn);
            Serial.println("[DEBUG END]");
            vTaskDelay(300);
            // MessageClear();
            if(_apn=="46692"){
                SendCommand(F("AT+CBANDCFG=\"NB-IOT\",8"));
                vTaskDelay(300);
                SendCommand(F("AT+CSTT=\"internet.iot\""));
                vTaskDelay(300);
                SendCommand(F("AT+CSQ"));
                vTaskDelay(300);
                Serial.println("Using CHT");
            }else if(_apn=="46697"){
                SendCommand(F("AT+CBANDCFG=\"NB-IOT\",3,28"));
                vTaskDelay(300);
                SendCommand(F("AT+CSTT=\"twm.nbiot\""));
                vTaskDelay(300);
                SendCommand(F("AT+CSQ"));
                vTaskDelay(300);
                Serial.println("Using TWM");

            }
            SendCommand(F("AT+CNACT=1"));
            vTaskDelay(300);
            SendCommand(F("AT+CIICR"));
            vTaskDelay(300);
            SendCommand(F("AT+CIFSR"));
            vTaskDelay(300);
            // SendCommand(F("AT+CIPMUX=1"));
            // vTaskDelay(300);
            

            pause_message = 1;

            
            SendCommand(F("AT+CNACT?"));
            vTaskDelay(300);
            if(Message[20]==1){
                pause_message = 0;
                vTaskDelay(300);
                return true;
            }else{
                pause_message = 0;
                vTaskDelay(300);
                return false;
            }


        }
        static void StartTCPConnection(const char *domain_name,const int port){
            String command="";
            command = "AT+CIPSTART=\"TCP\",\""+String(domain_name)+"\""+String(port);
            SendCommand(command);
            return;
        }
        static void StartUDPConnection(const char *domain_name,const int port){
            String command="";
            command = "AT+CIPSTART=\"UDP\",\""+String(domain_name)+"\""+String(port);
            SendCommand(command);
            return;
        }
        static void StartTCPConnection(byte ip1,byte ip2,byte ip3,byte ip4,const int port){
            String command="";
            command = "AT+CIPSTART=\"TCP\",\""+String(ip1)+"."+String(ip2)+"."+String(ip3)+"."+String(ip4)+"."+"\""+String(port);
            SendCommand(command);
            return;
        }
        static void StartUDPConnection(byte ip1,byte ip2,byte ip3,byte ip4,const int port){
            String command="";
            command = "AT+CIPSTART=\"UDP\",\""+String(ip1)+"."+String(ip2)+"."+String(ip3)+"."+String(ip4)+"."+"\""+String(port);
            SendCommand(command);
            return;
        }
        static void CloseConnection(){
            vTaskDelay(300);
            SendCommand("AT+CIPSHUT");
        }
        static void SendMessage(const char* message){
            
        }

};


#endif