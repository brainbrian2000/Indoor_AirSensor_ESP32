#ifndef _WIFI_CONNECT_CPP
    #define _WIFI_CONNECT_CPP
    #include <WiFi.h>
    #include <esp_wifi.h>
    #include <esp_err.h>
    // #include <esp_wpa.h>
    #include <esp_wpa2.h>
    #include <stdlib.h>
    #include <ESPping.h>
    #include <esp_netif_types.h>
    // #include <dns.h>
    /** 
     * Default Using in NTU and Using eduroam or SCplusNTU 
     * wifi in IPCS G206 Room.
     * Also can set by function by setting ssid, username and passwd.
     *  
    */
    // WiFiClass Wifi;
    class WiFi_connection{
        public:
            WiFi_connection(){};
            ~WiFi_connection(){};
            char peap_ssid_wpa2[31] = "eduroam";
            char peap_id_wpa2[32] = "b08209023@eduroam.ntu.edu.tw";
            char peap_username_wpa2[32] = "b08209023@eduroam.ntu.edu.tw";
            char peap_password_wpa2[32] = "Brianipcs206";
            char ssid_wpa2[31] = "SCplusNTU";
            char password_wpa2[32] = "smartcampus206";
            char default_ssid[32]="DISCONNECTED";
            char *current_ssid=default_ssid;
            int type=0;
            void changeWPA2(char const *ssid, char const *password){
                strcpy(ssid_wpa2,ssid);
                strcpy(password_wpa2,password);
                return;
            };
            void changePEAP(char const *ssid, char const *userid, char const *password){
                strcpy(peap_username_wpa2,userid);
                strcpy(peap_password_wpa2,password);
                strcpy(peap_ssid_wpa2,ssid);
                return;
            }



            void scanAP(void) {
                int n = WiFi.scanNetworks();
                delay(500);
                Serial.println("scan Wi-Fi done");
                if (n == 0)
                    Serial.println("no Wi-Fi networks found");
                else
                {
                    Serial.print(n);
                    Serial.println(" Wi-Fi networks found:");
                    for (int i = 0; i < n; ++i)
                    {
                    Serial.print(i + 1);
                    Serial.print(": ");
                    //印出SSID
                    Serial.print(WiFi.SSID(i));
                    Serial.print(" (");
                    //印出RSSI強度
                    Serial.print(WiFi.RSSI(i));
                    Serial.print(") ");
                    //印出加密模式
                    Serial.println(WiFi.encryptionType(i),HEX);
                    delay(10);
                    }
                }
            }       
            bool scanAP_check(char * ssid) {
                int n = WiFi.scanNetworks();
                bool check = false;
                delay(500);

                Serial.println("scan Wi-Fi done");
                if (n == 0)
                    Serial.println("no Wi-Fi networks found");
                else
                {
                    for (int i = 0; i < n; ++i)
                    {
                    if(WiFi.SSID(i).equals(String(ssid))){
                        check = true;
                    }
                    if(check){
                        Serial.println("Find WiFi name: "+String(ssid));
                        Serial.print("dB : "+String(WiFi.RSSI(i))+" Type: ");
                        Serial.println(WiFi.encryptionType(i),HEX);
                        return check;
                    }
                    }
                }
                return check;
            }     
            void printWiFiConfig(){
                Serial.print("Local IP     :");
                Serial.println((WiFi.localIP()));
                Serial.print("Mac Address  :");Serial.println(String(WiFi.macAddress()));
                Serial.print("Subnet Mask  :");Serial.println(WiFi.subnetMask());
                Serial.print("Gateway IP   :");Serial.println(WiFi.gatewayIP());
                Serial.print("DNS IP       :");Serial.println(WiFi.dnsIP());
                Serial.print("Broadcast IP :");Serial.println(WiFi.broadcastIP());
                Serial.print("Subnet CIDR  :");Serial.println(WiFi.subnetCIDR());
            }
            bool connect_peap(){
                WiFi.disconnect();
                WiFi.mode(WIFI_STA);
                ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
                ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_enable());
                ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)peap_id_wpa2, strlen(peap_id_wpa2)) );
                ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_username((uint8_t *)peap_username_wpa2, strlen(peap_username_wpa2)) );
                ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_password((uint8_t *)peap_password_wpa2, strlen(peap_password_wpa2)) );
                WiFi.begin(peap_ssid_wpa2);
                int counter = 0;
                bool return_status = false;
                current_ssid = peap_ssid_wpa2;
                Serial.println("Try connecting to wifi [peap]");
                while (WiFi.status() != WL_CONNECTED && counter < 30) {
                    delay(1500);
                    Serial.println("Connecting "+String(current_ssid));
                    counter++;
                }
                if(WiFi.status() == WL_CONNECTED){
                    return_status = true;
                    Serial.println("");
                    Serial.println("WiFi connected");

                }else{
                    return_status = false;
                    current_ssid = default_ssid;
                }
                return return_status;
            }
            bool connect_wpa2(){
                WiFi.disconnect();
                WiFi.mode(WIFI_STA);
                // WiFi.config(IPAddress(192,168,1,102),IPAddress(192,168,1,1),IPAddress(255,255,255,0),IPAddress(8,8,8,8),IPAddress(8,8,8,8));
                WiFi.begin(ssid_wpa2,password_wpa2);
                int counter = 0;
                bool return_status = false;
                current_ssid = ssid_wpa2;
                while (WiFi.status() != WL_CONNECTED && counter < 30) {
                    delay(1500);
                    Serial.println("Connecting "+String(current_ssid));
                    counter++;
                }
                if(WiFi.status() == WL_CONNECTED){
                    return_status = true;
                    Serial.println("");
                    Serial.println("WiFi connected");
                }else{
                    return_status = false;
                    current_ssid = default_ssid;
                }
                return return_status;
            }
            void turn_on_WiFi(int connection_type,int try_time=1){
                /**
                 * @brief Connection WiFi by Selection type
                 * @param connection_type 0 for only scan,1 for simple WPA2,2 for peap 
                 * 
                 */
                // Serial.println(WiFi.config(WiFi.localIP(),WiFi.gatewayIP(),WiFi.subnetMask(),IPAddress(8,8,8,8),IPAddress(8,8,8,8)));
                type = connection_type;
                int try_counter = 0;
                if(connection_type == 0){
                    scanAP();
                    return;
                }else if (connection_type == 1){
                    if(scanAP_check(ssid_wpa2)){
                        while(!connect_wpa2()&&try_counter < try_time){
                            try_counter++;        
                        }
                        printWiFiConfig();
                        PingTest();
                        return;
                    }
                }else if (connection_type == 2){
                    if(scanAP_check(peap_ssid_wpa2)){
                        while(!connect_peap()&& try_counter < try_time){
                            try_counter++;        
                        // }
                        printWiFiConfig();
                        PingTest();
                        return;
                        }
                    }
                }
            }
            void turn_off_WiFi(){
                type = 0;
                WiFi.disconnect(true);
                WiFi.mode(WIFI_OFF);
            }
            void connect_check(){
                while (WiFi.status() != WL_CONNECTED) {
                    Serial.println("====[RESETING WIFI]====");
                    turn_on_WiFi(type,1);
                }
            }

            bool PingTest(){
                // bool success = Ping.ping("www.google.com", 3);
                int ip[][4] = {
                    // {192,168,1,105},
                    // {192,168,1,1},
                    {8,8,8,8},
                    // {218,35,162,254},
                    // {192,168,1,102},
                    {168,95,1,1}
                };

                // IPAddress ip(8,8,8,8);
                // IPAddress ip(192,168,1,105);
                bool success;
                for(int i = 0; i <2; i++){
                    success = Ping.ping(IPAddress(ip[i][0], ip[i][1], ip[i][2], ip[i][3]), 3);
                    if(!success){
                        Serial.println("Ping failed");
                        // return false;
                        for(int j = 0; j <4;j++){
                            Serial.print(ip[i][j]);
                            Serial.print(' ');
                        }
                        Serial.println("");
                    }else{
                        Serial.println("Ping succesful.");
                        
                        for(int j = 0; j <4;j++){
                            Serial.print(ip[i][j]);
                            Serial.print(' ');
                        }
                        Serial.println("");
                    }
                }
                // const char *timeserver = "time.stdtime.gov.tw";
                // // const char *timeserver = "pool.ntp.org";
                // success = Ping.ping(timeserver, 3);
                // if(!success){
                //     Serial.println("Ping failed");
                //     Serial.print(timeserver);
                //     Serial.println("");
                // }else{
                //     Serial.println("Ping succesful.");
                //     Serial.print(timeserver);
                //     Serial.println("");
                // }

                
                return true;
            } 



    };
#endif