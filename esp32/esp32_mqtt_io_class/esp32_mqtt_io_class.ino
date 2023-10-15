#include <WiFi.h>
#include <PubSubClient.h>
#include "stdlib.h"
// #include <vector.h>
// WiFi
// const char *ssid = "SCplusNTU"; // Enter your WiFi name
// const char *password = "smartcampus206";  // Enter WiFi password
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

const char *ssid = "MakeNTU2023-B-2.4G"; // Enter your WiFi name
const char *password = "MakeNTU2023";  // Enter WiFi password
static String result="";
typedef class Proc_unit Proc_unit;
typedef class Sensor Sensor;
typedef class Pipe Pipe;
typedef class ctrl_er ctrl_er;
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
#define STD_DATA_FORMAT "da_temp:%f,hum:%f,press:%f,co2:%f,tvoc:%f,pm025:%d,pm100:%d"
#define DATA_INDEX 0
#define STD_CONTROL_FORMAT "c_"
#define CONTROL_INDEX 1
#define STD_SET1_FORMAT "s1_setid:%d,turn_on:%d,delete:%d,pipe:%d,sensor_in:%d,sensor_out:%d" //set up the pipe inside_sensor outside_sensor
#define SET1_INDEX 2
#define STD_SET2_FORMAT "s2_setid:%d,turn_on:%d,delete:%d,pipe:%d,sensor_in:%d,sensor_out:%d,pipe:%d,sensor_in:%d,sensor_out:%d" //set up the pipe inside_sensor outside_sensor
#define SET2_INDEX 3
#define STD_THRESHOLD_FORMAT "t_co2:%f,pm:%f,tvoc:%f"
#define THRESHOLD_INDEX 4
#define STD_WEIGHT_FORMAT "w_co2:%f,pm:%f,tvoc:%f"
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
        ~Proc_unit(){};
        void set_head(bool input_head){
            this->head = input_head;
            return;
        }
        void set_tail(bool input_tail){
            this->tail = input_tail;
            return;
        }
        void set_read_topic(String input){
            this->read_topic = input;
            return;
        }
        void set_send_topic(String input){
            this->send_topic = input;
            return;
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
            return;
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
# if 1
float co2_level1 = 1200;
float co2_level2 = 3000;
float pm_level1  = 25;
float pm_level2  = 200;
float tvoc_level1 = 25;
float tvoc_level2 = 500;
#endif
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
void clear_topic_arr(){
    for(int i =0;i<result_arr_size;i++){
        topic_arr[i] = "";
    }
            return;
}
void clear_msg_arr(){
    for(int i =0;i<result_arr_size;i++){
        msg_arr[i] = "";
    }
            return;
}
void clear_msg_type_arr(){
    for(int i =0;i<result_arr_size;i++){
        msg_type_arr[i] =0;
    }
            return;
}
void clear_result_arr(){
    for(int i =0;i<result_arr_size;i++){
        result_arr[i] = "";
    }
            return;
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
        ~ctrl_er(){};
        void set_floor_id(int floor_in,int id_in){
            // id_in.toCharArray(this->id,id_in.length()+1);
            // floor_in.toCharArray(this->floor,floor_in.length()+1);
            floor = floor_in;
            id  = id_in;
            return;
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
            // Serial.print("topic_subscribe = ");
            // Serial.println(topic_subscribe);

            return;
        }
        void set_mqtt(){
            // Serial.println("mqtt setting start");
            this->set_topic();
            this->mqtt_s.set_mqtt_topic(this->topic_subscribe);
            // this->mqtt_s.setup_mqtt();
            this->mqtt_p.set_mqtt_topic(this->topic_publish);
            // this->mqtt_p.setup_mqtt();
            // Serial.println("mqtt setting done");
            return;
        }
        void subscribe_mqtt(){
            // String temp;
            // temp = topic + "/receive";
            this->mqtt_s.subscribe();
            return;
            // this->mqtt_p.subscribe();
        }
        void publish_mqtt(String output_string){
            this->mqtt_p.publish(output_string);
            return;
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
        int   state = 0;
        char *topic_publish,*topic_subscribe;
        Sensor(){};
        ~Sensor(){};
        int co2_state(){
            if(_co2>co2_level2) return 2;
            else if(_co2>co2_level1) return 1;
            else return 0;
        };
        int pm_state(){
            if(_pm100>pm_level2) return 2;
            else if(_pm100>pm_level1) return 1;
            else return 0;
        };
        int tvoc_state(){
            if(_tvoc>tvoc_level2) return 2;
            else if(_tvoc>tvoc_level1) return 1;
            else return 0;
        };
        
        void update_state(){
            int co2=0, tvoc=0, pm=0, prev_state =0;
            co2 =   co2_state();
            tvoc =  tvoc_state();
            pm =    pm_state();
            // state = max(co2, tvoc, pm));
            prev_state = co2>=tvoc?co2:tvoc;
            state = prev_state>=pm?prev_state:pm;
            // return state;
            // state = co2>=(tvoc>=pm?tvoc:pm)?co2:(tvoc>=pm?tvoc:pm);
        };
        void print_data(){
            Serial.println("sensor id :"+String(id)+" "+String(_temp)+" "+String(_hum)+" "+String(_press)+" "+String(_co2)+" "+String(_tvoc)+" "+String(_pm100)+" state "+String(state));
        }
        void set_floor_id(int floor_in,int id_in){
            // id_in.toCharArray(this->id,id_in.length()+1);
            // floor_in.toCharArray(this->floor,floor_in.length()+1);
            floor = floor_in;
            id  = id_in;
            return;
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
            // Serial.print("topic_subscribe = ");
            // Serial.println(topic_subscribe);
            return;
        }
        void set_mqtt(){
            // Serial.println("mqtt setting start");
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
            // Serial.println(topic_publish);
            // this->mqtt_p.setup_mqtt();

            // Serial.println("mqtt setting done");
            return;
        }
        void subscribe_mqtt(){
            /**
             * @brief this function is only for control unit use 
             * 
             */
            this->mqtt_s.subscribe();
            Serial.println(mqtt_s.mqtt_topic);
            // this->mqtt_p.subscribe();
            return;
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
            return;
        }
        void read_msg(int id){
            /**
             * @brief  this function is only for contral unit use.
             * 
             */
            int __pm025,__pm100;
            float __temp,__hum,__press,__co2,__tvoc;    
            char msg[100];
            // strcpy(msg,msg_arr[id]);
            msg_arr[id].toCharArray(msg,msg_arr[id].length());
            sscanf(msg,STD_DATA_FORMAT,&__temp,&__hum,&__press,&__co2,&__tvoc,&__pm025,&__pm100);
            Serial.println(String(this->id)+"==="+String(__temp)+"   "+String(this->_temp)+"===");
            this->_temp = __temp;
            this->_hum = __hum;
            this->_press = __press;
            this->_co2 = __co2;
            this->_tvoc = __tvoc;
            this->_pm025 = __pm025;
            this->_pm100 = __pm100;
            update_state();
            return;
        }
        void reset_data(){
            _pm025=0;
            _pm100=0;
            _temp=0;
            _hum=0;
            _press=0;
            _co2=0;
            _tvoc=0;
            return;
        }
        void set_data(float temp, float hum, float press, float co2, float tvoc,int pm025,int pm100){
            _temp = temp;
            _hum = hum;
            _press = press;
            _co2 = co2;
            _tvoc = tvoc;
            _pm025 = pm025;
            _pm100 = pm100;
            return;
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
        int condition=0; //condition of the wall: is inner sensor better than outer sensor?
        Pipe(){};
        ~Pipe(){};
        void set_floor_id(int floor_in,int id_in){
            // id_in.toCharArray(this->id,id_in.length()+1);
            // floor_in.toCharArray(this->floor,floor_in.length()+1);
            floor = floor_in;
            id  = id_in;
            return;
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
            topic_publish = new char[temp.length()+1];
            topic_subscribe = new char[temp2.length()+1];
            // this part is reverse because it is for contoller
            temp.toCharArray(this->topic_publish,temp.length()+1);
            temp2.toCharArray(this->topic_subscribe,temp2.length()+1);
            // Serial.print("topic_subscribe = ");
            // Serial.println(topic_subscribe);
            return;
        }
        void set_mqtt(){
            // Serial.println("mqtt setting start");
            this->set_topic();
            this->mqtt_s.set_mqtt_topic(this->topic_subscribe);
            // this->mqtt_s.setup_mqtt();
            this->mqtt_p.set_mqtt_topic(this->topic_publish);
            // this->mqtt_p.setup_mqtt();
            // Serial.println("mqtt setting done");
            return;
        }
        void subscribe_mqtt(){
            // String temp;
            // temp = topic + "/receive";
            this->mqtt_s.subscribe();
            // Serial.println(mqtt_s.mqtt_topic);
            // this->mqtt_p.subscribe();
            return;
        }
        void publish_mqtt(String output_string){
            this->mqtt_p.publish(output_string);
            return;
        } 
        void read_msg(int id){
            char msg[100];
            msg_arr[id].toCharArray(msg,msg_arr[id].length());
            sscanf(msg,STD_PIPE_FORMAT,&door_in,&door_out,&fan_in,&fan_out,&press_in,&press_out);
        
            return;
        }
        void publish_mqtt(){
            char msg[100];
            sprintf(msg,STD_PIPE_FORMAT,door_in,door_out,fan_in,fan_out,press_in,press_out);
            this->mqtt_p.publish(String(msg));
            Serial.println(mqtt_p.mqtt_topic);
            return;
        }
        void set_state(int _door_in, int _door_out, int _fan_in, int _fan_out){
            door_in = _door_in;
            door_out = _door_out;
            fan_in = _fan_in;
            fan_out = _fan_out;
            publish_mqtt();
        }
        void print_state(){
            Serial.println("pipe id : "+String(id)+" door in/out fan in/out : "+String(door_in)+"/"+String(door_out)+"  "+String(fan_in)+"/"+String(fan_out));
        }
};

class Sensor sensor_arr[DEVICE_ARRAY_SIZE];
class Pipe pipe_arr[DEVICE_ARRAY_SIZE];
float co2_weight = 1, pm_weight = 1, tvoc_weight = 1;
  /*user defined thresholes*/
  /*weightings for compare*/

  /*condition array: -1, close; 1, open; 0, compare*/
int condition_array[3][3] = {{-1,1,1},{-1,0,1},{-1,-1,0}};

/*calculate threshole according to sensors*/
int calculate_threshole(Sensor sensor_i, Sensor sensor_o){
    int result = 0; //too high : open, inside-outside.
    result += (sensor_i._co2 - sensor_o._co2)*co2_weight/800;
    result += (sensor_i._pm100 - sensor_o._pm100)*pm_weight/70;
    result += (sensor_i._tvoc - sensor_o._tvoc)*tvoc_weight/100;
    if(result>3) return 1;
    else return -1;
    }
    /*compare 2 sensor states*/
int compare_sensors(Sensor sensor_i, Sensor sensor_o){
    int condition = condition_array[sensor_o.state][sensor_i.state];
    //condition:compare
    if(!condition) condition = calculate_threshole(sensor_i,sensor_o);
    return condition;
}
/*single pipe control logic*/
//assign states according to current input value
//directly update pipe status
void update_single_pipe_state(Pipe &pipe, Sensor sensor_i, Sensor sensor_o){
    pipe.press_in = sensor_i._press;
    pipe.press_out = sensor_i._press;
    int condition = compare_sensors(sensor_i, sensor_o);
        switch(condition){
            case -1: //close
            pipe.set_state(0,0,0,0);
            return;
            case 1:  //open
            pipe.set_state(1,1,1,1);
            return;
        }
}
/*two pipe control logic: consider convection*/
void update_two_pipe_status(Pipe &pipe1, Sensor sensor_i1, Sensor sensor_o1, Pipe &pipe2, Sensor sensor_i2, Sensor sensor_o2){
    pipe1.press_in = sensor_i1._press;
    pipe2.press_in = sensor_i2._press;
    pipe1.press_out = sensor_i1._press;
    pipe2.press_out = sensor_i2._press;
    
    pipe1.condition = compare_sensors(sensor_i1, sensor_o1);
    pipe2.condition = compare_sensors(sensor_i2, sensor_o2);

    if(pipe1.condition == -1 && pipe2.condition == -1){ //inner better: close all pipes to advoid energy loss
        pipe1.set_state(0,0,0,0);
        pipe2.set_state(0,0,0,0);
        return;
    }
    else if(pipe1.condition == 1 && pipe2.condition == -1){
        //determine pipe1 state
        if(pipe1.press_out > pipe1.press_in) pipe1.set_state(1,0,0,0);
        else pipe1.set_state(1,0,1,0);
        //determine pipe2 state
        if(pipe2.press_in > pipe2.press_out) pipe2.set_state(0,1,0,0);
        else pipe2.set_state(0,1,0,1);
        return;
    }
    else if(pipe1.condition == -1 && pipe2.condition == 1){
        //determine pipe1 state
        if(pipe1.press_in > pipe1.press_out) pipe1.set_state(0,1,0,0);
        else pipe1.set_state(0,1,0,1);
        //determine pipe2 state
        if(pipe2.press_out > pipe2.press_in) pipe2.set_state(1,0,0,0);
        else pipe2.set_state(1,0,1,0);
        return;
    }
    else{
        pipe1.set_state(1,1,1,1);
        pipe2.set_state(1,1,1,1);
        return;
    }
}




class pipe_process_unit{
    public:
    
    pipe_process_unit(){};
    ~pipe_process_unit(){};
    int pipe1=999,pipe2=999;
    int s1_1=999,s1_2=999,s2_1=999,s2_2=999;
    int on_state=0;
    int del_state = 1;
    int type_of_set = 0;//1 for one pipe 2 for 2 pipe
    int process_id=999;
    void set_del(int input){
        this->del_state = input;
        return;
    }
    void set_on(int input){
        this->on_state = input;
        return;
    } 
    void update_process(int p_type,int p_id,int p_on,int p_del,int p1,int s11,int s12,int p2,int s21,int s22){
        this->type_of_set = p_type;
        this->process_id = p_id;
        this->on_state = p_on;
        this->del_state = p_del;
        this->pipe1 = p1;
        this->pipe2 = p2;
        this->s1_1 = s11;
        this->s1_2 = s12;
        this->s2_2 = s22;
        this->s2_1 = s21;
        return;
    }
    // int check_block_can_update
    void update_state(){
        if(process_id!=999)Serial.println(" update_state of process id : " +String(process_id));
        if(on_state==1 && del_state==0){
            /**
             * @brief is on and not deleted 
             * 
             */
            if(type_of_set == 1){
                Serial.println("==ONE PIPE==\n"+String(pipe1)+" "+String(s1_1)+" "+String(s1_2));
                update_single_pipe_state(pipe_arr[pipe1],sensor_arr[s1_1],sensor_arr[s1_2]);
                pipe_arr[pipe1].print_state();
                sensor_arr[s1_1].print_data();
                sensor_arr[s1_2].print_data();
            }else if (type_of_set == 2){
                Serial.println("==TWO PIPE==\n"+String(pipe1)+" "+String(s1_1)+" "+String(s1_2)+" "+String(pipe2)+" "+String(s2_1)+" "+String(s2_2));
                update_two_pipe_status(pipe_arr[pipe1],sensor_arr[s1_1],sensor_arr[s1_2],pipe_arr[pipe2],sensor_arr[s2_1],sensor_arr[s2_2]);
                pipe_arr[pipe1].print_state();
                sensor_arr[s1_1].print_data();
                sensor_arr[s1_2].print_data();
                pipe_arr[pipe2].print_state();
                sensor_arr[s2_1].print_data();
                sensor_arr[s2_2].print_data();
            }
        }
    }
};

class pipe_process_unit process_arr[result_arr_size];
void set_process(int index,ctrl_er controler){
    char topic[100];
    char check_topic[100];
    topic_arr[index].toCharArray(check_topic,topic_arr[index].length()+1);
    strcpy(topic,controler.mqtt_s.mqtt_topic);
    int pipe1=999,pipe2=999;
    int s1_1=999,s1_2=999,s2_1=999,s2_2=999;
    int on_state=0;
    int del_state = 1;
    int type_of_set = 0;//1 for one pipe 2 for 2 pipe
    int process_id=999;
    // read the topic of */control/*
    // Serial.println("IN")
    if(strstr(check_topic,topic)!=NULL){

        // #define STD_SET1_FORMAT "s1_setid:%d,turn_on:%d,delete:%d,pipe:%d,sensor_in:%d,sensor_out:%d" //set up the pipe inside_sensor outside_sensor
        // #define SET1_INDEX 2
        // #define STD_SET2_FORMAT "s2_setid:%d,turn_on:%d,delete:%d,pipe:%d,sensor_in:%d,sensor_out:%d,pipe:%d,sensorin:%d,sensor:%d" //set up the pipe inside_sensor outside_sensor
        // #define SET2_INDEX 3
        char msg_in_arr[100];
        msg_arr[index].toCharArray(msg_in_arr,msg_arr[index].length()+1);
        // Serial.println("===[IN PROCESS SET]====");
        if(msg_in_arr[0]=='s'){
            if(msg_in_arr[1]=='1'){
                sscanf(msg_in_arr,STD_SET1_FORMAT,&process_id,&on_state,&del_state,&pipe1,&s1_1,&s1_2);
                type_of_set=1;
                for(int i=0;i<result_arr_size;i++){
                    if((process_arr[i].process_id == process_id)&& process_id != 999){
                        process_arr[i].update_process(type_of_set,process_id,on_state,del_state,pipe1,s1_1,s1_2,pipe2,s2_1,s2_2);
                        if(process_id!=0)Serial.println("Set at block "+String(i)+" with process id = "+String(process_id));
                        return;
                    }
                }
                for(int i=0;i<result_arr_size;i++){
                    if(process_arr[i].del_state==1){
                        process_arr[i].update_process(type_of_set,process_id,on_state,del_state,pipe1,s1_1,s1_2,pipe2,s2_1,s2_2);
                        Serial.println("Set at block "+String(i)+" with process id = "+String(process_id));
                        return;
                    }
                }
            }else if(msg_in_arr[1]=='2'){
                sscanf(msg_in_arr,STD_SET2_FORMAT,&process_id,&on_state,&del_state,&pipe1,&s1_1,&s1_2,&pipe2,&s2_1,&s2_2);
                type_of_set=2;
                for(int i=0;i<result_arr_size;i++){
                    if((process_arr[i].process_id == process_id)&& process_id!=999){
                        process_arr[i].update_process(type_of_set,process_id,on_state,del_state,pipe1,s1_1,s1_2,pipe2,s2_1,s2_2);
                        Serial.println("Set at block "+String(i)+" with process id = "+String(process_id));
                        return;
                    }
                }
                for(int i=0;i<result_arr_size;i++){
                    if(process_arr[i].del_state==1){
                        process_arr[i].update_process(type_of_set,process_id,on_state,del_state,pipe1,s1_1,s1_2,pipe2,s2_1,s2_2);
                        Serial.println("Set at block "+String(i)+" with process id = "+String(process_id));
                        return;
                    }
                }
            }
            Serial.println("===[FAIL TO UPDATE LOGIC]===");
        }else{
            return;
        }
    }
}

void process_process(int id){
    // Serial.print("Process id : "+String(id));
    process_arr[id].update_state();
}

class read_command{
    public:
    read_command(){};  
    ~read_command(){};  
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
            return;
    }
    void clear(){
        this->topic = "";
        this->msg = "";
            return;
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
            return;
    }
    int me_index(Proc_unit &input_block){
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
            Serial.println(check_topic);
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
        
        // Serial.print("[out] Readed topic is : ");
        // Serial.println(check_topic);
        if(strstr(check_topic,topic)!=NULL){
            if(strstr(topic,"subscribe")){
                return 9999;
            }
            // char *temp[100];
            // Serial.print("[in] Readed topic is : ");
            // Serial.println(check_topic);
            strcat(topic,"%d/publish");
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
};



void read_state_update(int index,class ctrl_er controler){
    char topic[100];
    char check_topic[100];
    topic_arr[index].toCharArray(check_topic,topic_arr[index].length()+1);
    strcpy(topic,controler.mqtt_s.mqtt_topic);
    if(strstr(check_topic,topic)!=NULL){
        char msg_in_arr[100];
        msg_arr[index].toCharArray(msg_in_arr,msg_arr[index].length()+1);
        Serial.println("===[IN WEIGHT UPDATE]====");
        if(msg_in_arr[0]=='w'){
            sscanf(msg_in_arr,STD_WEIGHT_FORMAT,&co2_weight,&pm_weight,&tvoc_weight);
            Serial.print("Now weight : CO2 ");
            Serial.print(co2_weight);
            Serial.print(" PM ");
            Serial.print(pm_weight);
            Serial.print(" TVOC ");
            Serial.println(tvoc_weight);
        }else if(msg_in_arr[0] == 't'){
            sscanf(msg_in_arr,STD_THRESHOLD_FORMAT,&co2_level1,&pm_level1,&tvoc_level1);
            Serial.print("Now threshold : CO2 ");
            Serial.print(co2_level1);
            Serial.print(" PM ");
            Serial.print(pm_level1);
            Serial.print(" TVOC ");
            Serial.println(tvoc_level1);
        }

        // Serial.print(String(sprintf(topic,"co2%f,pm%f,tvoc_%f",co2_weight,pm_weight,tvoc_weight)));
            return;
    }


}




void sensor_arr_init(){
    for (int i=0;i<DEVICE_ARRAY_SIZE;i++){
        sensor_arr[i].set_floor_id(1,i);
        sensor_arr[i].reset_data();
        sensor_arr[i].set_topic();
        sensor_arr[i].set_mqtt();
        // sensor_arr[i].subscribe_mqtt();
        sensor_arr[i].set_data(23,80,1006.26,480,582,25,63);
        // sensor_arr[i].
        // sensor_arr[i].
    }
            return;
}
void pipe_arr_init(){
    for(int i=0;i<DEVICE_ARRAY_SIZE;i++){
        pipe_arr[i].set_floor_id(1,i);
        pipe_arr[i].set_topic();
        pipe_arr[i].set_mqtt();
        // pipe_arr[i].subscribe_mqtt();
    }
    return;
}
void print_free_memory()
{
    Serial.println("[APP] Free memory: " + String(esp_get_free_heap_size()) + " bytes");
    // DEBUGLF("\n>>>---------- Momory Info ----------<<<");
    // DEBUGL2("Free Heap: ", ESP.getFreeHeap());
    // DEBUGL2("Free PSRAM: ", ESP.getFreePsram());
    // DEBUGL2("Free sketch sapce: ", ESP.getFreeSketchSpace());
    // DEBUGL2("Heap Size: ", ESP.getHeapSize());
    // DEBUGL2("Max Alloc Heap: ", ESP.getMaxAllocHeap());
    // DEBUGL2("Max Alloc Psram: ", ESP.getMaxAllocPsram());
    // DEBUGL2("Psram Size: ", ESP.getPsramSize());
    // DEBUGLF(">>>---------- Momory Info End ----------<<<\n");
}
unsigned long currentmillis = 0;
unsigned long storagemillis = 0;
unsigned long storagemillis2 = 0;
int interval= 6000;
read_command command_processer;
ctrl_er id1;
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
    mqtt.set_mqtt(_mqtt_broker,_mqtt_topic,_mqtt_username,_mqtt_password,_mqtt_port);
    mqtt.setup_mqtt();
    mqtt.subscribe();
    sensor_arr_init();
    pipe_arr_init();
    id1.set_mqtt();
    mqtt.subscribe("NTU/riceball/#");
    id1.subscribe_mqtt();
    currentmillis = millis();
    // if want to subscribe all messeage under some topic need using this statement to subscribe.
    // if set the other mqtt using # is danger because it cannot send messages with #.
    // delay(5000);
}
#define DEBUG_MESSAGE_PRINT 1
unsigned long time_wifi=0;
void connect_check(){
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
            sensor_arr_init();
            pipe_arr_init();
            id1.set_mqtt();
            mqtt.subscribe("NTU/riceball/#");
            id1.subscribe_mqtt();
            currentmillis = millis();
        }
}
 
void loop() {
    currentmillis = millis();
    if(currentmillis-storagemillis>interval){
        print_free_memory();
        storagemillis = millis();
        connect_check();
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
            if(currentmillis-storagemillis2>interval*4){
                for(int i =0;i<5;i++){
                    // sensor_arr[i]._temp = sensor_arr[i]._temp+1;
                    // pipe_arr[i].door_in = pipe_arr[i].door_in+1;
                    sensor_arr[i].publish_mqtt();
                    // sensor_arr[i].publish_mqtt(String("Pipe messeage sending test."));
                    pipe_arr[i].publish_mqtt();
                    // sensor_arr[i]._temp = sensor_arr[i]._temp-1;
                    // pipe_arr[i].door_in = pipe_arr[i].door_in-1;
                }
                storagemillis2 = millis();
            }

        #endif
        
        time_wifi = millis();
        /*
        read the message limit until the read messeage is null.
        ->read all unread msg at broker.
        */

        Serial.println("====[DEBUG]==== RECEIVE PART");
        // delay(1000);  
        connect_check();
        mqtt.mqtt_read_broker_message();
        // Serial.println("====[DEBUG]==== RECEIVE PART1");

        command_processer.read_into_array();
        // Serial.println("====[DEBUG]==== RECEIVE PART2");
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



        // Serial.println("====[DEBUG]==== SENSOR UPDATE PART");
        // delay(1000);
        int index_of_sensor=0;
        for(int i = 0; i < result_arr_size; i++){
            if(!result_arr[i].equals("")){
                index_of_sensor = command_processer.read_sensor(i);
                if(index_of_sensor ==9999){
                    continue;
                }else{
                    // Serial.println("====[DEBUG]==== temp before  "+String(sensor_arr[index_of_sensor]._temp));
                    sensor_arr[index_of_sensor].read_msg(i);
                    // Serial.print("index "+String(index_of_sensor)+" ");
                    // Serial.println("====[DEBUG]==== temp after   "+String(sensor_arr[index_of_sensor]._temp));
                }
            }else{
                break;
            }
        } 

        // Serial.println("====[DEBUG]==== PIPE UPDATE PART");
        // delay(1000);
        // /*
        int index_of_pipe =0;
        for(int i = 0; i < result_arr_size; i++){
            if(!result_arr[i].equals("")){
                index_of_pipe = command_processer.read_pipe(i);
                if(index_of_pipe ==9999){
                    continue;
                }else{
                    pipe_arr[index_of_pipe].read_msg(i);
                }
            }else{
                break;
            }
        } 
        // */
        // Serial.println("====[DEBUG]==== weight and threshold setting");
        // delay(1000);
        for(int i = 0; i < result_arr_size; i++){
            read_state_update(i,id1);
        }

        Serial.println("====[DEBUG]==== logic block setting");
        for(int i=0;i<result_arr_size;i++){
            set_process(i,id1);
        }
        Serial.println("====[DEBUG]==== logic block PROCESSING");
        for(int i=0;i<result_arr_size;i++){
            process_process(i);
        }






        // Serial.println("====[DEBUG]==== threshold setting");
    
        
        
        
        
        
        
        
        // /*
        
        // */
        // delay(1000);
        // Serial.println("====[DEBUG]====0");
        clear_result_arr();
        // Serial.println("====[DEBUG]====1");
        clear_msg_arr();
        // Serial.println("====[DEBUG]====2");
        clear_topic_arr();
        // Serial.println("====[DEBUG]====3");
        clear_msg_type_arr();
        // Serial.println("====[DEBUG]====4");
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
            sensor_arr_init();
            pipe_arr_init();
            id1.set_mqtt();
            mqtt.subscribe("NTU/riceball/#");
            id1.subscribe_mqtt();
            currentmillis = millis();
        }
    }
}
