#include <WiFi.h>
#include <PubSubClient.h>

#include "Wire.h"
#include <SPI.h>
#include <stdlib.h>
#include <SD.h>
// #include <I2C_RTC.h>
// #include <LowPower.h>
// WiFi
// const char *ssid = "CHIEH-HSIANGFAN"; // Enter your WiFi name
// const char *password = "brainbrian2000";  // Enter WiFi password
const char *ssid = "SCplusNTU"; // Enter your WiFi name
const char *password = "smartcampus206";  // Enter WiFi password
#define DEBUG_MSG_SEND 1
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
static String result="";
#define result_arr_size 100
String result_arr[result_arr_size];
/*
void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
    result += (char) payload[i];
 }
 Serial.println();
 Serial.println("-----------------------");
}
*/
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
        }
        MQTT(char *broker,char * topic){this->mqtt_broker = broker;this->mqtt_topic = topic;}
        // MQTT(char *broker,char * topic,int port);
        // MQTT(char *broker,char * topic,char *username,int port);
        // MQTT(char *broker,char * topic,char *username,char *password,int port);
        // ~MQTT();
        
        void set_mqtt(char *broker,char * topic,char *username,char *password,int port){
            mqtt_port = port;
            mqtt_password = password;
            mqtt_username = username;
            mqtt_broker = broker;
            mqtt_topic = topic;
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
        }
        void publish(String outputstr){
            char * temp = new char[outputstr.length()+1];
            outputstr.toCharArray(temp, outputstr.length()+1);

            mqttclient.publish(mqtt_topic,temp);
            delete temp;
        }
        void subscribe(){
            mqttclient.subscribe(mqtt_topic);
        }
        void subscribe(char* temp){
            mqttclient.subscribe(temp);
        }
        void unsubscribe(){
            mqttclient.unsubscribe(mqtt_topic);
        }
        void unsubscribe(char *temp){
            mqttclient.unsubscribe(temp);
        }
        void clear_result_arr(){
            for(int i =0;i<result_arr_size;i++){
                result_arr[i] = "";
            }
        }
        String read_result_arr(int i){
            return result_arr[i];
        }
        void mqtt_read_broker_message(){
            for (int i = 0; i <result_arr_size;i++){
                mqttclient.loop();
                if(result==""){
                    break;
                }
            }
        }
};



MQTT mqtt;
MQTT mqtt2;
void setup() {
 // Set software serial baud to 115200;
 Serial.begin(115200);
 // connecting to a WiFi network
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.println("Connecting to WiFi..");
 }
    Serial.println("Connected to the WiFi network");
    // mqtt2.set_mqtt(_mqtt_broker,"NTU/riceball/testlalala",_mqtt_username,_mqtt_password,_mqtt_port);
    // mqtt2.setup_mqtt();
    // mqtt2.subscribe();
    mqtt.set_mqtt(_mqtt_broker,_mqtt_topic,_mqtt_username,_mqtt_password,_mqtt_port);
    mqtt.setup_mqtt();
    mqtt.subscribe();
    // if want to subscribe all messeage under some topic need using this statement to subscribe.
    // if set the other mqtt using # is danger because it cannot send messages with #.
    mqtt.subscribe("NTU/riceball/#");

}


void loop() {
    delay(1000);  
    #if DEBUG_MSG_SEND
        /*
        Send testing messeage to broker 
        */
        for (int i = 0; i <6;i++){
            mqtt.publish("this is messeage "+String(i));//send with setted topic.
            mqtt2.publish("this is messeage "+String(i));//send with setted topic.
        }
    #endif
    
    /*
    read the message limit until the read messeage is null.
    ->read all unread msg at broker.
    */
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
                }
            }
        }
        else{
            Serial.println("====[DEBUG]==== Result array has nothing. ====[DEBUG MEG END]====");
        }
    #endif
    
    
    // /*
    
    // */
    // delay(1000);
    mqtt.clear_result_arr();
}
