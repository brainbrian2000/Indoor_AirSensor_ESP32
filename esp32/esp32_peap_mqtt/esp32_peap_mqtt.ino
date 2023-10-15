#include <WiFi.h>
#include <PubSubClient.h>
#include "stdlib.h"
#include "esp_wpa2.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include <WiFi.h>
// WiFi
// const char *ssid = "SCplusNTU"; // Enter your WiFi name
// const char *password = "smartcampus206";  // Enter WiFi password
#define EXAMPLE_WIFI_SSID "eduroam"
#define EXAMPLE_EAP_ID "b08209023@eduroam.ntu.edu.tw"
#define EXAMPLE_EAP_USERNAME "b08209023@eduroam.ntu.edu.tw"
#define EXAMPLE_EAP_PASSWORD "Brianipcs206"
#define EXAMPLE_EAP_METHOD 1
#define DEBUG_MSG_SEND 0
#define DEBUG_MSG_RECV 1
// MQTT Broker->if want to set it, 
// need call the function in class MQTT and call it by constructor.
// this is initial statement of mqtt to avoid send some garbages and broke other programs.
char *_mqtt_broker = "scplus-iot.ipcs.ntu.edu.tw";
char *_mqtt_topic = "NTU/riceball/makentu2023";
char *_mqtt_username = "brainbrian2000";
char *_mqtt_password = "test";
int _mqtt_port = 1883;
WiFiClient espClient;
PubSubClient mqttclient(espClient);


#define result_arr_size 100
String result_arr[result_arr_size];
static String result = " ";
void peap_wifi_setting(){
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
//   esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
  ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
  ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_enable());
  ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EXAMPLE_EAP_ID, strlen(EXAMPLE_EAP_ID)) );
  ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EXAMPLE_EAP_USERNAME, strlen(EXAMPLE_EAP_USERNAME)) );
  ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EXAMPLE_EAP_PASSWORD, strlen(EXAMPLE_EAP_PASSWORD)) );
  // scanAP();
  WiFi.begin(EXAMPLE_WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1500);
    Serial.println("Connecting...");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

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
      Serial.print(")");
      //印出加密模式
      Serial.println(WiFi.encryptionType(i),HEX);
      delay(10);
     }
  }
}
class MQTT{
    /** Message
     * this class need static PubSubClient named mqttclient
     * just the mqtt saved properties and to let code cleaner.
    */
    public:
        // const char *mqtt_broker = "35.194.169.153";
        // char *mqtt_broker = "scplus-iot.ipcs.ntu.edu.tw";
        // char *mqtt_topic = "NTU/riceball/makentu2023";
        // char *mqtt_username = "testing";
        // char *mqtt_password = "test";
        // int mqtt_port = 1883;
        char *mqtt_broker = _mqtt_broker;
        char *mqtt_topic = _mqtt_topic;
        char *mqtt_username = _mqtt_username;
        char *mqtt_password = _mqtt_password;
        int mqtt_port = _mqtt_port;
        MQTT(){
            // static PubSubClient mqttclient(espClient);
        };
        MQTT(char *broker,char * topic){this->mqtt_broker = broker;this->mqtt_topic = topic;}
        // MQTT(char *broker,char * topic,int port);
        // MQTT(char *broker,char * topic,char *username,int port);
        // MQTT(char *broker,char * topic,char *username,char *password,int port);
        // ~MQTT();
        ~MQTT(){};
        void set_mqtt(char *broker,char * topic,char *username,char *password,int port){
            mqtt_port = port;
            mqtt_password = password;
            mqtt_username = username;
            mqtt_broker = broker;
            mqtt_topic = topic;
            return;
        }
        void set_mqtt_topic(char *topic){
            this->mqtt_topic = topic;
            return;
            }
        static void callback_mqtt(char *topic, byte *payload, unsigned int length) {
            // Serial.print("Message arrived in topic: ");
            // Serial.println(topic);
            // Serial.print("Message:");
            result = "";
            for (int i = 0; i < length; i++) {
                // Serial.print((char) payload[i]);
                result += (char) payload[i];
            }
            for(int j =0;j<result_arr_size;j++){
                if(result_arr[j].equals("")){
                    result_arr[j] = String(topic)+"->"+result;
                    break;
                }
                // else{
                    // Serial.println("next_arr_index = "+String(j));
                // }
            }
            // result = "";
            // Serial.println();
            // Serial.println("-----------------------");
            // return result;
            return;
        }
        void setup_mqtt(){
            mqttclient.setServer(mqtt_broker, mqtt_port);
            mqttclient.setCallback(callback_mqtt);
            while (!mqttclient.connected()) {
                String client_id = "esp32-client-";
                client_id += String(WiFi.macAddress());
                Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
                if (mqttclient.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
                    Serial.println("====== MQTT broker connected Der La======");
                } else {
                    Serial.print("failed with state, reconnecting");
                    Serial.print(mqttclient.state());
                    delay(2000);
                }
            }
            return;
        }
        void publish(String outputstr){
            char * temp = new char[outputstr.length()+1];
            outputstr.toCharArray(temp, outputstr.length()+1);
            // Serial.print("Sending:");
            // Serial.print(mqtt_topic);
            // Serial.print(":");
            // Serial.println(temp);
            mqttclient.publish(mqtt_topic,temp);
            delete temp;
            return;
        }
        void subscribe(){
            mqttclient.subscribe(mqtt_topic);
            return;
        }
        void subscribe(char* temp){
            mqttclient.subscribe(temp);
            return;
        }
        void unsubscribe(){
            mqttclient.unsubscribe(mqtt_topic);
            return;
        }
        void unsubscribe(char *temp){
            mqttclient.unsubscribe(temp);
            return;
        }
        void mqtt_read_broker_message(){
            for (int i = 0; i <result_arr_size;i++){
                mqttclient.loop();
                if(result==""){
                    break;
                }
            }
            return;
        }
};

unsigned long time_wifi = 0;
unsigned long currentmillis = 0;
unsigned long storagemillis = 0;
unsigned long interval = 5000;
MQTT mqtt;
void connect_check(){
    time_wifi = millis();
        while (WiFi.status() != WL_CONNECTED) {
            // WiFi.begin(ssid, password);
            scanAP();
            peap_wifi_setting();
            Serial.println("====[RESETING WIFI]====");
            while(millis()-time_wifi<interval){
            }
                time_wifi = millis();
                Serial.println("Connecting to WiFi..");
        }
        while (!mqttclient.connected()) {
            Serial.println("====[RESETING WIFI]====");
            mqtt.setup_mqtt();
            // mqtt.subscribe();
            // mqtt.subscribe("NTU/riceball/#");
            currentmillis = millis();
        }
}
void clear_result_arr(){
    for(int i =0;i<result_arr_size;i++){
        result_arr[i] = "";
    }
            return;
}
void setup() {
  Serial.begin(115200);
  Serial.println("INIT");
  scanAP();
  peap_wifi_setting();
//   mqtt = &(new MQTT());
  mqtt.setup_mqtt();
  // This part of the code is taken from the oficial wpa2_enterprise example from esp-idf
  // ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
  // ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_enable() );
  currentmillis = millis();

}
String publish_s;
unsigned int i = 0;
void loop(){
    currentmillis = millis();
    if((currentmillis-storagemillis) > interval){
        storagemillis = millis();
        i++;
        // Serial.println(currentmillis);
        // Serial.println(millis());
        connect_check();
        Serial.println("Sending message...");
        publish_s = "publish_testing"+String(i)+"_form_"+EXAMPLE_WIFI_SSID;
        mqtt.publish(publish_s);
        Serial.println(publish_s);
        mqtt.mqtt_read_broker_message();
        #if DEBUG_MSG_RECV
            /*
            print the message in array, this part will be rewrite by the other application,
            like read airquality state or upload pipe state.
            messeage format: [topic]->[messeage]
            */
            if(!result_arr[0].equals("")){
                for (int i = 0; i <result_arr_size; i++){
                    if(result_arr[i].equals("")){
                        Serial.println("====[DEBUG]==== total messeage count is "+String(i)+" in result array\n\n");
                        break;
                    }
                    else{
                        Serial.println("====[DEBUG]==== messege in array "+result_arr[i]);
                        // Serial.println("====[DEBUG]==== [topic]"+topic_arr[i]);
                        // Serial.println("====[DEBUG]==== [ msg ]"+msg_arr[i]);
                    }
                }
            }
            else{
                Serial.println("====[DEBUG]==== Result array has nothing. ====[DEBUG MEG END]====");
            }
        #endif
        clear_result_arr();
        
    }
}