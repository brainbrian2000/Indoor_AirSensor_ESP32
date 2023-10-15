#include "esp_wpa2.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include <WiFi.h>

#define EXAMPLE_WIFI_SSID "ntu_peap"
#define EXAMPLE_EAP_METHOD 1

#define EXAMPLE_EAP_ID "b08209023"
#define EXAMPLE_EAP_USERNAME "b08209023"
#define EXAMPLE_EAP_PASSWORD "Brian168"
void peap_wifi_setting(){
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  // esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
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
#define EXAMPLE_WIFI_SSID "ntu_peap"
void scanAP(void) {
  int n = WiFi.scanNetworks();
  delay(1000);
  Serial.println("scan Wi-Fi done");
  if (n == 0)
    Serial.println("no Wi-Fi networks found");
  else
  {
    Serial.print(n);
    Serial.println(" Wi-Fi networks found");
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
void setup() {
  Serial.begin(115200);
  Serial.println("INIT");
  scanAP();
  peap_wifi_setting();
  // This part of the code is taken from the oficial wpa2_enterprise example from esp-idf
  // ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
  // ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_enable() );

}

void loop() {
  // put your main code here, to run repeatedly:

}
