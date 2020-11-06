/**
   ESPNOW - Basic communication - Slave
   Date: 26th September 2017
   Author: Arvind Ravulavaru <https://github.com/arvindr21>
   Purpose: ESPNow Communication between a Master ESP32 and a Slave ESP32
   Description: This sketch consists of the code for the Slave module.
   Resources: (A bit outdated)
   a. https://espressif.com/sites/default/files/documentation/esp-now_user_guide_en.pdf
   b. http://www.esploradores.com/practica-6-conexion-esp-now/

   << This Device Slave >>

   Flow: Master
   Step 1 : ESPNow Init on Master and set it in STA mode
   Step 2 : Start scanning for Slave ESP32 (we have added a prefix of `slave` to the SSID of slave for an easy setup)
   Step 3 : Once found, add Slave as peer
   Step 4 : Register for send callback
   Step 5 : Start Transmitting data from Master to Slave

   Flow: Slave
   Step 1 : ESPNow Init on Slave
   Step 2 : Update the SSID of Slave with a prefix of `slave`
   Step 3 : Set Slave in AP mode
   Step 4 : Register for receive callback and wait for data
   Step 5 : Once data arrives, print it in the serial monitor

   Note: Master and Slave have been defined to easily understand the setup.
         Based on the ESPNOW API, there is no concept of Master and Slave.
         Any devices can act as master or salve.
*/

#include <esp_now.h>
#include <WiFi.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define CHANNEL 1

typedef struct struct_message {
    int16_t AccX;
    int16_t AccY;
    int16_t AccZ;
} struct_message;

struct_message MPU6050Readings;

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() != ESP_OK) {
    ESP.restart();
  }
}

// config AP SSID
void configDeviceAP() {
  const char *SSID = "BaseStation_1";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", CHANNEL, 0);
}

void setup() {

  // SD card setup
  while(!SD.begin()){
    delay(1000);
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    return;
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);

  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);
}

void appendFile(fs::FS &fs, const char * path, int16_t message){
    File file = fs.open(path, FILE_APPEND);
    if(!file){
        return;
    }
    file.println(message);
    file.close();
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  struct_message incomingReadings;
  memcpy(&incomingReadings, data, sizeof(incomingReadings));
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  
  appendFile(SD, "/AccX.txt", incomingReadings.AccX);
  appendFile(SD, "/AccY.txt", incomingReadings.AccY);
  appendFile(SD, "/AccZ.txt", incomingReadings.AccZ);

}

void loop() {
  // Chill
}
