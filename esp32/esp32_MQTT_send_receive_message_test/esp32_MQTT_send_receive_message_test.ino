/*
This program can send messages to web server and get message from web server using MQTT.
And check the time needs for sending message and gap between finish sending message and finish receiving message.

Before uploading the program to esp32, please check if your Wifi name and Wifi password is correct.
*/

/*
Codes below are refering from 
"https://www.survivingwithandroid.com/esp32-mqtt-client-publish-and-subscribe/#google_vignette"

We can monitor the messages sent from esp32 via
"https://www.hivemq.com/demos/websocket-client/"
After getting into this website, press connect (there's no need to change any thing),
then press "Add New Topic Subscription" button,
type in Topic name (inside "mqttClient.publish(Topic name, words)" ),
we will get messages.
*/

/*
Need to do:
  1. This is a public server, we need to use our local server with password.
  2. Add sensor codes 
  3. Some codes to deal with the data from sensor
  4. Sending and receiving SEQUENCE to prevent from data conflict.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

/*
BME280 sensor libraries, we won't use them, but we will use other sensor, so I keep them in the code.
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
*/

const char *SSID = "SCplusNTU"; // Wifi name
const char *PWD = "smartcampus206";  //Wifi password

/*
Adafruit_BME280 bme;
*/

long last_time = 0;
static int times = 0;

static unsigned long int gap = 0; //Time gap from finishing sending texts till finishing receiving texts
static unsigned long int gapFront = 0;
static unsigned long int gapBack = 0;

static unsigned long int gap2 = 0;//Time that it takes to send texts
static unsigned long int gap2Front = 0;
static unsigned long int gap2Back = 0;

/*
char data[100];
*/

// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 

char *mqttServer = "scplus-iot.ipcs.ntu.edu.tw"; //MQTT server name
int mqttPort = 1883;  //MQTT port


void connectToWiFi() {
  Serial.print("Connectiog to ");
 
  WiFi.begin(SSID, PWD);
  Serial.println(SSID);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("Connected.");
  
} 

//Subscribe to MQTT topic to listen to incoming message
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("\nCallback - ");
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

//Time gap from finishing sending texts till finishing receiving texts
  gapBack = millis();
  gap = gapBack - gapFront; 
  Serial.println();
  Serial.print("Time gap from finishing sending texts till finishing receiving texts = ");
  Serial.print(gap);  

}

//Set up and configure the ESP32 Client using PubSubClient
void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  // set the callback function
  mqttClient.setCallback(callback);
}


void setup() {
  Serial.begin(115200);
  
  connectToWiFi();
/*
Code to check if the sensor is connected
  if (!bme.begin(0x76)) {
    Serial.println("Problem connecting to BME280");
  }
*/
  setupMQTT();
    
}

//Connect the ESP32 to the MQTT broker
void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // subscribe to topic
        mqttClient.subscribe("NTU/riceball/makentu_test2");
        mqttClient.subscribe("/testword");
      }
      
  }
}


void loop() {

  if (!mqttClient.connected())
    reconnect();

  mqttClient.loop();  //getting message from the web server
  long now = millis();
  String words = "Here is esp32 MQTT test by rcbfan, count ";
  char pub[100];
  if (now - last_time > 1000) {
    // Send data
    times += 1;
    words += String(times);
    words.toCharArray(pub, 100);

  //Time that it takes to send texts
    gap2Front = millis();
    mqttClient.publish("NTU/riceball/makentu_test2", pub); //publish the message to the web server
    gap2Back = millis();
    gap2 = gap2Back - gap2Front;
    Serial.println();
    Serial.print("Time takes to send texts = ");
    Serial.print(gap2);

    gapFront = millis();
    last_time = now;
  
  //check the message which is sent
    Serial.println();
    Serial.print(pub);
    
  }
  if(times>=100){
    times=0;
  }
/*
some code to send sensor data to MQTT

  long now = millis();
  if (now - last_time > 60000) {
    // Send data
    float temp = bme.readTemperature();
    float hum = bme.readHumidity();
    float pres = bme.readPressure() / 100;
    

    // Publishing data throgh MQTT
    sprintf(data, "%f", temp);
    Serial.println(temp);
    mqttClient.publish("/swa/temperature", data);
    sprintf(data, "%f", hum);
    Serial.println(hum);
    mqttClient.publish("/swa/humidity", data);
    sprintf(data, "%f", pres);
    Serial.println(pres);
    mqttClient.publish("/swa/pressure", data);
    last_time = now;
  
  }
*/

}