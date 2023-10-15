#include <WiFi.h>

// Set these to your desired credentials.
const char *ssid     = "ESP32_wifi_brainbrian2000"; //設定一組網路名稱(ssid)
const char *password = "brainbrian2000"; //設定一組網路密碼(pasword)

IPAddress local_IP(192, 168, 1, 1);  
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void setup() {
  Serial.begin(115200);
  delay(2000);
  WiFi.mode(WIFI_AP);
  Serial.println();
  Serial.println("Configuring soft-AP...");

  WiFi.softAPConfig(local_IP,gateway,subnet);
  WiFi.softAP(ssid, password);

  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("softAP macAddress: ");
  Serial.println(WiFi.softAPmacAddress());
  Serial.println("Server started.....");
}

void loop() {
}