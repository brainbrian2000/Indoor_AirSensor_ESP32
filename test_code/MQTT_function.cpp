#ifndef MQTT_FUNCTION_CPP
#define MQTT_FUNCTION_CPP
#include <WiFi.h>
#include <PubSubClient.h>
#include <stdlib.h>

static char *_mqtt_broker = "scplus-iot.ipcs.ntu.edu.tw";
static char *_mqtt_topic = "NTU/riceball/airsensor";
static char *_mqtt_username = "brainbrian2000";
static char *_mqtt_password = "test";
static int _mqtt_port = 1883;
#define result_arr_size 100
static String result_arr[result_arr_size];
static String result = "";
static WiFiClient espClient;
static PubSubClient mqttclient(espClient);

class MQTT_function{
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
        MQTT_function(){
            // static PubSubClient mqttclient(espClient);
        };
        MQTT_function(char *broker,char * topic){
            this->mqtt_broker = broker;
            this->mqtt_topic = topic;
        };
        // MQTT(char *broker,char * topic,int port);
        // MQTT(char *broker,char * topic,char *username,int port);
        // MQTT(char *broker,char * topic,char *username,char *password,int port);
        ~MQTT_function(){};
        
        void set_mqtt(char *broker,char * topic,char *username,char *password,int port){
            mqtt_port = port;
            mqtt_password = password;
            mqtt_username = username;
            mqtt_broker = broker;
            mqtt_topic = topic;
            Serial.println(espClient.remoteIP().toString());
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
            if(WiFi.status() != WL_CONNECTED){
              return;
            }
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
                    // break;
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



#endif