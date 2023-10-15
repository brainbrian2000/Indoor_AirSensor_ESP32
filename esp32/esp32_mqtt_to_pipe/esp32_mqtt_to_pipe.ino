#include <WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include "stdlib.h"
// #include <vector.h>
// WiFi
const char *ssid = "MakeNTU2023-A-2.4G"; // Enter your WiFi name
const char *password = "MakeNTU2023";  // Enter WiFi password
// const char *ssid = "SCplusNTU"; // Enter your WiFi name
// const char *password = "smartcampus206";  // Enter WiFi password
#define DEBUG_MSG_SEND 0
#define DEBUG_MSG_RECV 1
#define USING_BME280 0
// MQTT Broker->if want to set it, 
// need call the function in class MQTT and call it by constructor.
// this is initial statement of mqtt to avoid send some garbages and broke other programs.
char *_mqtt_broker = "scplus-iot.ipcs.ntu.edu.tw";
char *_mqtt_topic = "NTU/riceball/makentu2023";
char *_mqtt_username = "brainbrian2000";
char *_mqtt_password = "test";
int _mqtt_port = 1883;

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4; 

#if USING_BME280
  #include <Adafruit_Sensor.h>
  #include <Adafruit_BME280.h>
  Adafruit_BME280 bme;
#endif

WiFiClient espClient;
PubSubClient mqttclient(espClient);
static String result="";
typedef class Proc_unit Proc_unit;
typedef class Sensor Sensor;
typedef class Pipe Pipe;
#define result_arr_size 100
String result_arr[result_arr_size];
String msg_arr[result_arr_size];
String topic_arr[result_arr_size];
int msg_type_arr[result_arr_size];
#define DEVICE_ARRAY_SIZE 36
// pipe pipe_arr[DEVICE_ARRAY_SIZE];
#define CTRL_ER 1
#define SENSOR 0
#define PIPE 0
int self_id = 1;
#define STD_DATA_FORMAT "da_temp:%2.1f,hum:%3.1f,press:%4.1f,co2:%4.1f,tvoc:%4.1f,pm025:%d,pm100:%d"
#define DATA_INDEX 0
#define STD_CONTROL_FORMAT "c_"
#define CONTROL_INDEX 1
#define STD_SET1_FORMAT "s1_setid:%d,pipe%d,sensor_in%d,sensor_out%d" //set up the pipe inside_sensor outside_sensor
#define SET1_INDEX 2
#define STD_SET2_FORMAT "s2_setid:%d,pipe:%d,sensor_in:%d,sensor_out:%d,pipe:%d,sensorin:%d,sensor:%d" //set up the pipe inside_sensor outside_sensor
#define SET2_INDEX 3
#define STD_THRESHOLD_FORMAT "t_co2:%f,pm%f,tvoc:%f"
#define THRESHOLD_INDEX 4
#define STD_WEIGHT_FORMAT "w_co2:%f,pm%f,tvoc:%f"
#define WEIGHT_INDEX 5
#define STD_DELETE_FORMAT "de_setid:%d"
#define DELETE_INDEX 6
#define STD_PIPE_FORMAT "p_door_in:%d,door_out:%d,fan_in:%d,fan_out:%d,press_in:%f,press_out:%f"
class Proc_unit{
    public:
        Proc_unit *prev,*next;
        bool head=0,tail=0;
        String read_topic="";
        String send_topic="";
        Proc_unit(){};
        void set_head(bool input_head){
            this->head = input_head;
        }
        void set_tail(bool input_tail){
            this->tail = input_tail;
        }
        void set_read_topic(String input){
            this->read_topic = input;
        }
        void set_send_topic(String input){
            this->send_topic = input;
        }
};
int read_command_type(int index){
    if(msg_arr[index][0]=='d'&&msg_arr[index][1]=='a'){
        return DATA_INDEX;
    }else if(msg_arr[index][0]=='c'){
        return CONTROL_INDEX;
    }else if(msg_arr[index][0]=='s'&&msg_arr[index][1] == '1'){
        return SET1_INDEX;
    }else if(msg_arr[index][0]=='s'&&msg_arr[index][1] == '2'){
        return SET2_INDEX;
    }else if(msg_arr[index][0]=='t'){
        return THRESHOLD_INDEX;
    }else if(msg_arr[index][0]=='w'){
        return WEIGHT_INDEX;
    }else if(msg_arr[index][0]=='d'&&msg_arr[index][1]=='e'){
        return DELETE_INDEX;
    }
}
void set_type_index(){
    for(int i=0; i<result_arr_size;i++){
        if(msg_arr[i]==""){break;}
        else{
            msg_type_arr[i] = read_command_type(i);
        }
    }
}


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
        void set_mqtt_topic(char *topic){this->mqtt_topic = topic;}
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
            // Serial.print("Sending:");
            // Serial.print(mqtt_topic);
            // Serial.print(":");
            // Serial.println(temp);
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
        void mqtt_read_broker_message(){
            for (int i = 0; i <result_arr_size;i++){
                mqttclient.loop();
                if(result==""){
                    break;
                }
            }
        }
};
void clear_topic_arr(){
    for(int i =0;i<result_arr_size;i++){
        topic_arr[i] = "";
    }
}
void clear_msg_arr(){
    for(int i =0;i<result_arr_size;i++){
        msg_arr[i] = "";
    }
}
void clear_msg_type_arr(){
    for(int i =0;i<result_arr_size;i++){
        msg_type_arr[i] =0;
    }
}
void clear_result_arr(){
    for(int i =0;i<result_arr_size;i++){
        result_arr[i] = "";
    }
}

String read_result_arr(int i){
    return result_arr[i];
}

class ctrl_er{
    public:
        MQTT mqtt_s,mqtt_p;    
        char *where = "NTU/riceball/makentu2023/";
        char *type = "control/";
        int   floor = 1;
        int   id = self_id;
        char *topic_publish,*topic_subscribe;
        ctrl_er(){};
        void set_floor_id(int floor_in,int id_in){
            // id_in.toCharArray(this->id,id_in.length()+1);
            // floor_in.toCharArray(this->floor,floor_in.length()+1);
            floor = floor_in;
            id  = id_in;
        };
        void set_topic(){
            String temp,temp2;
            char temp_charf[10] = {'\0'};
            char temp_chari[10] = {'\0'};
            itoa(floor,temp_charf,10);
            itoa(id,temp_chari,10);
            temp += where;
            temp += type;
            temp += temp_charf;
            temp +="F/";
            temp += temp_chari;
            temp2 =temp + "/publish";
            temp = temp + "/subscribe";
            topic_publish = new char[temp2.length()+1];
            topic_subscribe = new char[temp.length()+1];
            temp2.toCharArray(this->topic_publish,temp2.length()+1);
            temp.toCharArray(this->topic_subscribe,temp.length()+1);
            Serial.print("topic_subscribe = ");
            Serial.println(topic_subscribe);
        }
        void set_mqtt(){
            Serial.println("mqtt setting start");
            this->set_topic();
            this->mqtt_s.set_mqtt_topic(this->topic_subscribe);
            // this->mqtt_s.setup_mqtt();
            this->mqtt_p.set_mqtt_topic(this->topic_publish);
            // this->mqtt_p.setup_mqtt();
            Serial.println("mqtt setting done");
        }
        void subscribe_mqtt(){
            // String temp;
            // temp = topic + "/receive";
            this->mqtt_s.subscribe();
            // this->mqtt_p.subscribe();
        }
        void publish_mqtt(String output_string){
            this->mqtt_p.publish(output_string);
        }

};
class Sensor{
    public:
    /**
     * @brief In controller the data is for storage state to compare function.
     * In sensor, the data is for sensor unit storage its data and send data to MQTT broker by std format. 
     *  
     */
        MQTT mqtt_s,mqtt_p;
        int _pm025,_pm100;
        float _temp,_hum,_press,_co2,_tvoc;    
        char *where = "NTU/riceball/makentu2023/";
        char *type = "sensor/";
        int   floor = 1;
        int   id = self_id;
        char *topic_publish,*topic_subscribe;
        Sensor(){};

        void set_floor_id(int floor_in,int id_in){
            // id_in.toCharArray(this->id,id_in.length()+1);
            // floor_in.toCharArray(this->floor,floor_in.length()+1);
            floor = floor_in;
            id  = id_in;
        }
        void set_topic(){
            String temp,temp2;
            char temp_charf[10] = {'\0'};
            char temp_chari[10] = {'\0'};
            itoa(floor,temp_charf,10);
            itoa(id,temp_chari,10);
            temp += where;
            temp += type;
            temp += temp_charf;
            temp +="F/";
            temp += temp_chari;
            temp2 = temp;
            // temp = temp + "/subscribe";
            topic_publish = new char[temp2.length()+1];
            topic_subscribe = new char[temp.length()+1];
            temp2.toCharArray(this->topic_publish,temp2.length()+1);
            temp.toCharArray(this->topic_subscribe,temp.length()+1);
            Serial.print("topic_subscribe = ");
            Serial.println(topic_subscribe);
        }
        void set_mqtt(){
            Serial.println("mqtt setting start");
            this->set_topic();
            this->mqtt_s.set_mqtt_topic(this->topic_subscribe);
            // this->mqtt_s.setup_mqtt();
            this->mqtt_p.set_mqtt_topic(this->topic_publish);
            // Serial.println("====");
            // Serial.println(mqtt_s.mqtt_topic);
            // Serial.println("====");
            // Serial.println(topic_subscribe);
            // Serial.println("====");
            // Serial.println(mqtt_p.mqtt_topic);
            // Serial.println('====");
            Serial.println(topic_publish);
            // this->mqtt_p.setup_mqtt();

            Serial.println("mqtt setting done");
        }
        void subscribe_mqtt(){
            /**
             * @brief this function is only for control unit use 
             * 
             */
            this->mqtt_s.subscribe();
            // this->mqtt_p.subscribe();
        }
        void publish_mqtt(){
            /**
             * @brief  this function is only for sensor unit use.
             * 
             */
            char msg[100];
            sprintf(msg,STD_DATA_FORMAT,_temp,_hum,_press,_co2,_tvoc,_pm025,_pm100);
            // Serial.println("==="+String(msg)+"===");
            this->mqtt_p.publish(String(msg));
        }
        void read_msg(int id){
            /**
             * @brief  this function is only for contral unit use.
             * 
             */
            char msg[100];
            // strcpy(msg,msg_arr[id]);
            msg_arr[id].toCharArray(msg,msg_arr[id].length());
            sscanf(msg,STD_DATA_FORMAT,&_temp,&_hum,&_press,&_co2,&_tvoc,&_pm025,&_pm100);

        }
        void reset_data(){
            _pm025=0;
            _pm100=0;
            _temp=0;
            _hum=0;
            _press=0;
            _co2=0;
            _tvoc=0;
        }
        void set_data(float temp, float hum, float press, float co2, float tvoc,int pm025,int pm100){
            _temp = temp;
            _hum = hum;
            _press = press;
            _co2 = co2;
            _tvoc = tvoc;
            _pm025 = pm025;
            _pm100 = pm100;
        }

};
class Pipe{
    public:
        MQTT mqtt_s,mqtt_p;    
        char *where = "NTU/riceball/makentu2023/";
        char *type = "pipe/";
        int   floor = 1;
        int   door_in=0,door_out=0; //state of pipe door, in is the pipe suck the air into indoor.
        int   fan_in =0,fan_out=0;  //state of pipe fan
        float press_in=1013.25,press_out=1013.25; //pressure inside and outside
        int   id = self_id;
        char *topic_publish,*topic_subscribe;
        Pipe(){};
        void set_floor_id(int floor_in,int id_in){
            // id_in.toCharArray(this->id,id_in.length()+1);
            // floor_in.toCharArray(this->floor,floor_in.length()+1);
            floor = floor_in;
            id  = id_in;
        };
        void set_topic(){
            String temp,temp2;
            char temp_charf[10] = {'\0'};
            char temp_chari[10] = {'\0'};
            itoa(floor,temp_charf,10);
            itoa(id,temp_chari,10);
            temp += where;
            temp += type;
            temp += temp_charf;
            temp +="F/";
            temp += temp_chari;
            temp2 =temp + "/publish";
            temp = temp + "/subscribe";
            topic_publish = new char[temp2.length()+1];
            topic_subscribe = new char[temp.length()+1];
            temp2.toCharArray(this->topic_publish,temp2.length()+1);
            temp.toCharArray(this->topic_subscribe,temp.length()+1);
            Serial.print("topic_subscribe = ");
            Serial.println(topic_subscribe);
        }
        void set_mqtt(){
            Serial.println("mqtt setting start");
            this->set_topic();
            this->mqtt_s.set_mqtt_topic(this->topic_subscribe);
            // this->mqtt_s.setup_mqtt();
            this->mqtt_p.set_mqtt_topic(this->topic_publish);
            // this->mqtt_p.setup_mqtt();
            Serial.println("mqtt setting done");
        }
        void subscribe_mqtt(){
            // String temp;
            // temp = topic + "/receive";
            this->mqtt_s.subscribe();
            // this->mqtt_p.subscribe();
        }
        void publish_mqtt(String output_string){
            this->mqtt_p.publish(output_string);
        } 
        void read_msg(int id){
            char msg[100];
            msg_arr[id].toCharArray(msg,msg_arr[id].length());
            sscanf(msg,STD_PIPE_FORMAT,&door_in,&door_out,&fan_in,&fan_out,&press_in,&press_out);
        
        }
        void publish_mqtt(){
            char msg[100];
            sprintf(msg,STD_PIPE_FORMAT,door_in,door_out,fan_in,fan_out,press_in,press_out);
            this->mqtt_p.publish(String(msg));
        }
        void set_door_fan(){
          if(door_in==0){
            servo1.write(0); //angle set by tested
            servo2.write(0);
          }
          else if(door_in==1){
            servo1.write(90);
            servo2.write(90);
          }
          if(door_out==0){
            servo3.write(5);
            servo4.write(0);
          }
          else if (door_out==1){
            servo3.write(105);
            servo4.write(90);
          }
        }
};



class read_command{
    public:
    String topic,msg;
    void split_command(String command){
        /**
         * @brief input command type is [topic]->[messeage],
         * using this function let this class's msg and topic be update. 
         * 
         */
        char *command_to_str = new char[command.length()+1];
        char *splited;
        command.toCharArray(command_to_str, command.length()+1);
        splited = strtok(command_to_str,"->");
        this->topic = String(splited);
        splited = strtok(NULL,"->");
        this->msg = String(splited);
        // Serial.println("get [topic]: " + this->topic+"[msg]:"+this->msg);
        // delete splited;
        delete command_to_str;
    }
    void clear(){
        this->topic = "";
        this->msg = "";
    }
    void read_into_array(){
        for (int i = 0; i < result_arr_size;i++){
            if(result_arr[i] == ""){
                break;
            }else{
                this->clear();
                this->split_command(result_arr[i]);
                topic_arr[i] = this->topic;
                msg_arr[i] = this->msg;
            }
        }
    }
    int me_index(Proc_unit input_block){
        for(int i = 0; i < result_arr_size; i++){
            if(result_arr[i].equals(input_block.read_topic)){
                return i;
            }
        }
    }
    int read_sensor(int index){
        int output_index=9999;
        char topic[100];
        strcpy(topic,_mqtt_topic);
        strcat(topic,"/sensor/1F/");
        // strcat(topic,char('0'+index));
        char check_topic[100];
        topic_arr[index].toCharArray(check_topic,topic_arr[index].length()+1);
        
        // Serial.print("[out] Readed topic is : ");
        // Serial.println(check_topic);
        if(strstr(check_topic,topic)!=NULL){
            // char *temp[100];
            // Serial.print("[in] Readed topic is : ");
            // Serial.println(check_topic);
            strcat(topic,"%d");
            sscanf(check_topic,topic,&output_index);
            // Serial.println("read_index:"+String(output_index));
        }
        if(output_index>=DEVICE_ARRAY_SIZE){
            return 9999;
        }
        else{
            return output_index;
        }
        
    }
    int read_pipe(int index){
        int output_index=9999;
        char topic[100];
        strcpy(topic,_mqtt_topic);
        strcat(topic,"/pipe/1F/");
        char check_topic[100];
        topic_arr[index].toCharArray(check_topic,topic_arr[index].length()+1);
        
        Serial.print("[out] Readed topic is : ");
        Serial.println(check_topic);
        if(strstr(check_topic,topic)!=NULL){
            if(strstr(topic,"subscribe")){
                return 9999;
            }
            // char *temp[100];
            Serial.print("[in] Readed topic is : ");
            Serial.println(check_topic);
            strcat(topic,"%d/publish");
            sscanf(check_topic,topic,&output_index);
            Serial.println("read_index:"+String(output_index));
        }
        if(output_index>=DEVICE_ARRAY_SIZE){
            return 9999;
        }
        else{
            return output_index;
        }
    }
};


int floor_in = 1;
int id_in = 22;


read_command command_processer;
ctrl_er id1;
MQTT mqtt;
MQTT mqtt2;
Pipe pipes;

int servo1_pin = 26;
int servo2_pin = 25;
int servo3_pin = 33;
int servo4_pin = 5;

int fan1_pin = 32;
int fan2_pin = 27;

int last_time = 0;

 // 建立一個 servo 物件，最多可建立 12個 servo

// int pos1 = 0; 
// int pos2 = 0; 
// int pos3 = 0; 
// int pos4 = 0; 


void setup() {
 // Set software serial baud to 115200;
    Serial.begin(115200);
 // connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
   // sensor_arr_init();
    //pipe_arr_init();

    Serial.println("Connected to the WiFi network");
    // mqtt.set_mqtt(_mqtt_broker,_mqtt_topic,_mqtt_username,_mqtt_password,_mqtt_port);
    // mqtt.setup_mqtt();
    // mqtt.subscribe();

    // if want to subscribe all messeage under some topic need using this statement to subscribe.
    // if set the other mqtt using # is danger because it cannot send messages with #.
    // mqtt.subscribe("NTU/riceball/#");


    pipes.set_floor_id(floor_in, id_in);
    pipes.set_topic();
    pipes.set_mqtt();
    pipes.subscribe_mqtt();

    // pinMode(servo1_pin, OUTPUT);
    // pinMode(servo2_pin, OUTPUT);
    // pinMode(servo3_pin, OUTPUT);
    // pinMode(servo4_pin , OUTPUT);
    servo1.attach(servo1_pin);
    servo2.attach(servo2_pin);
    servo3.attach(servo3_pin);
    servo4.attach(servo4_pin);

    pinMode(fan1_pin, OUTPUT);
    pinMode(fan2_pin, OUTPUT);

    #if USING_BME280
      bme.begin(0x76, &Wire);
    #endif
}
    int interval = 6000;
    unsigned long time_wifi=0;


void loop() {
    Serial.println("====[DEBUG]==== SENDING PART");
    #if DEBUG_MSG_SEND
        /*
        Send testing messeage to broker 
        */
        /*
        for (int i = 0; i <6;i++){
            id1.publish_mqtt("this is messeage "+String(i));//send with setted topic.
            delay(500);
            // mqtt2.;publish("this is messeage "+String(i));//send with setted topic.
        }
        */
        for(int i =0;i<5;i++){
            sensor_arr[i]._temp = sensor_arr[i]._temp+1;
            sensor_arr[i].publish_mqtt();
            // sensor_arr[i].reset_data();
            pipe_arr[i].publish_mqtt(String("Pipe messeage sending test."));
            pipe_arr[i].publish_mqtt();
        }

    #endif
    
    /*
    read the message limit until the read messeage is null.
    ->read all unread msg at broker.
    */
    // Serial.println("====[DEBUG]==== RECEIVE PART");
    mqtt.mqtt_read_broker_message();
    command_processer.read_into_array();


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
    // 
    delay(1000);
    
    // Serial.println("====[DEBUG]==== SENSOR UPDATE PART");
    // int index_of_sensor=0;
    // for(int i = 0; i < result_arr_size; i++){
    //     if(!result_arr[i].equals("")){
    //         index_of_sensor = command_processer.read_sensor(i);
    //         if(index_of_sensor ==9999){
    //             continue;
    //         }else{
    //             // Serial.println("====[DEBUG]==== temp before  "+String(sensor_arr[i]._temp));
    //             sensor_arr[index_of_sensor].read_msg(i);
    //             // Serial.println("====[DEBUG]==== temp after   "+String(sensor_arr[i]._temp));
    //         }
    //     }else{
    //         break;
    //     }
    // } 

    // Serial.println("====[DEBUG]==== PIPE UPDATE PART");
    int index_of_pipe =0;
    for(int i = 0; i < result_arr_size; i++){
        Serial.println(pipes.topic_subscribe);
        if(!result_arr[i].equals("") && topic_arr[i] == pipes.topic_subscribe){
          
          Serial.println(msg_arr[i]);
          int s_len = msg_arr[i].length();
          String tmp_str = msg_arr[i];
          char msg_c[msg_arr[i].length() + 1];
          msg_arr[i].toCharArray(msg_c, s_len);

          // pos: 10, 21, 30, 40
          pipes.door_in = msg_c[10] - '0';
          pipes.door_out = msg_c[21] - '0';
          pipes.fan_in = msg_c[30] - '0';
          pipes.fan_out = msg_c[40] - '0';
          Serial.println(msg_c[0]);

            // index_of_pipe = command_processer.read_pipe(i);
            // if(index_of_pipe ==9999){
            //     continue;
            // }else{
            //     // pipe_arr[index_of_pipe].read_msg(i);
            //     //i ->你要的msg存的位子
            //     Serial.println(msg_arr[i]);
            // }
        }else{
            break;
        }
    } 
    // /*
    // Serial.println(pipes.mqtt_p.mqtt_topic);
    if(pipes.door_in == 0)
      pipes.door_in == 1000;

    // Serial.println("==topic subscibe==");
    // Serial.println(pipes.topic_subscribe);
    // Serial.println("==topic subscibe==");
    // pin servo1:5, servo2:6

    // digitalWrite(servo1_pin, pipes.door_in);
    // digitalWrite(servo2_pin, pipes.door_in);
    // digitalWrite(servo3_pin, pipes.door_out);
    // digitalWrite(servo4_pin, pipes.door_out);
    pipes.set_door_fan();
    if(pipes.fan_in ==0){
        digitalWrite(fan1_pin, LOW);
    }else{
        digitalWrite(fan1_pin, HIGH);
    }
    if(pipes.fan_out ==0){
        digitalWrite(fan2_pin, LOW);
    }else{
        digitalWrite(fan2_pin, HIGH);
    }

    // digitalWrite(fan2_pin, pipes.fan_out);

    
    if(millis() - last_time> 10000){
      #if USING_BME280
        pipes.press_out = bme.readPressure() / 100.0F;
      #endif
      pipes.publish_mqtt();
      last_time = millis();
    }

    // */
    clear_result_arr();
    clear_msg_arr();
    clear_topic_arr();
    clear_msg_type_arr();
    delay(1000);  
    time_wifi = millis();
        while (WiFi.status() != WL_CONNECTED) {
            WiFi.begin(ssid, password);
            Serial.println("====[RESETING WIFI]====");
            while(millis()-time_wifi<interval){
            }
                time_wifi = millis();
                Serial.println("Connecting to WiFi..");
        }

      while (!mqttclient.connected()) {
          Serial.println("====[RESETING WIFI]====");
          mqtt.setup_mqtt();
          mqtt.subscribe();
          // // sensor_arr_init();
          // // pipe_arr_init();
          // id1.set_mqtt();
          mqtt.subscribe("NTU/riceball/#");
          // id1.subscribe_mqtt();
          // currentmillis = millis();
      }
}