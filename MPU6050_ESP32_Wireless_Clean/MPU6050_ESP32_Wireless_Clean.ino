/**
   ESPNOW - Basic communication - Master
   Date: 26th September 2017
   Author: Arvind Ravulavaru <https://github.com/arvindr21>
   Purpose: ESPNow Communication between a Master ESP32 and a Slave ESP32
   Description: This sketch consists of the code for the Master module.
   Resources: (A bit outdated)
   a. https://espressif.com/sites/default/files/documentation/esp-now_user_guide_en.pdf
   b. http://www.esploradores.com/practica-6-conexion-esp-now/

   << This Device Master >>

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
#include <Wire.h>

// Global copy of slave
esp_now_peer_info_t slave;
#define CHANNEL 3
#define DELETEBEFOREPAIR 0

const byte accelerometerAddress = 0x68;

bool isPaired = false;

typedef struct struct_message{
    int16_t AccX;
    int16_t AccY;
    int16_t AccZ;
} struct_message;

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() != ESP_OK) {
    ESP.restart();
  }
}

// Scan for slaves in AP mode
void ScanForSlave() {
  int8_t scanResults = WiFi.scanNetworks();
  // reset on each scan
  bool slaveFound = 0;
  memset(&slave, 0, sizeof(slave));

  if (scanResults != 0) {
    for (int i = 0; i < scanResults; ++i) {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);
      String BSSIDstr = WiFi.BSSIDstr(i);

      delay(10);
      // Check if the current device starts with `BaseStation`
      if (SSID.indexOf("BaseStation") == 0) {
        // Get BSSID => Mac Address of the Slave
        int mac[6];
        if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] ) ) {
          for (int ii = 0; ii < 6; ++ii ) {
            slave.peer_addr[ii] = (uint8_t) mac[ii];
          }
        }

        slave.channel = CHANNEL; // pick a channel
        slave.encrypt = 0; // no encryption

        slaveFound = 1;
        // we are planning to have only one slave in this example;
        // Hence, break after we find one, to be a bit efficient
        break;
      }
    }
  }

  // clean up ram
  WiFi.scanDelete();
}

// Check if the slave is already paired with the master.
// If not, pair the slave with master
bool manageSlave() {
  if (slave.channel == CHANNEL) {
    if (DELETEBEFOREPAIR) {
      deletePeer();
    }

    // check if the peer exists
    bool exists = esp_now_is_peer_exist(slave.peer_addr);
    if ( exists) {
      // Slave already paired.
      return true;
    } else {
      // Slave not paired, attempt pair
      esp_err_t addStatus = esp_now_add_peer(&slave);
      if (addStatus == ESP_OK) {
        // Pair success
        return true;
      } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
        return true;
      } else {
        return false;
      }
    }
  } else {
    return false;
  }
}

void deletePeer() {
  esp_err_t delStatus = esp_now_del_peer(slave.peer_addr);
}

// send data
void sendData(struct_message data) {
  const uint8_t *peer_addr = slave.peer_addr;
  esp_err_t result = esp_now_send(peer_addr, (uint8_t *) &data, sizeof(data));
}

// callback when data is sent from Master to Slave
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  if (status != ESP_NOW_SEND_SUCCESS) {
    isPaired = false;
  }
}

struct_message readAccelerometer () {
  struct_message data;
  
  Wire.beginTransmission(accelerometerAddress);
  Wire.write(0x3B); // start at ACCEL_XOUT_H
  Wire.endTransmission(false);

  // request registers ACCEL_XOUT_H through ACCEL_ZOUT_L
  Wire.requestFrom(accelerometerAddress, 6, true); // request 6 bytes

  // read data into memory
  if (Wire.available() == 6) {
    data.AccX = (Wire.read() << 8 | Wire.read());
    data.AccY = (Wire.read() << 8 | Wire.read());
    data.AccZ = (Wire.read() << 8 | Wire.read());
  }
  Wire.endTransmission(true);
  return data;
}

void setup() {
  //Set device in STA mode to begin with
  WiFi.mode(WIFI_STA);
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // configure accelerometer
  Wire.begin();
  Wire.beginTransmission(accelerometerAddress);
  Wire.write(0x1B); // select GYRO_CONFIG
  Wire.write(0x08); // write 00001000, no self test, +/- 4g
  Wire.endTransmission();

  Wire.beginTransmission(accelerometerAddress);
  Wire.write(0x6B); // select PWR_MGMT_1
  Wire.write(0x00); // write 000000000, turn off sleep mode use 8MHz oscilator
  Wire.endTransmission();
}

void loop() {
  while (!isPaired) { // not connected search untill found
    // In the loop we scan for slave
    ScanForSlave();
    // If Slave is found, it would be populate in `slave` variable
    // We will check if `slave` is defined and then we proceed further
    if (slave.channel == CHANNEL) { // check if slave channel is defined
      // `slave` is defined
      // Add slave as peer if it has not been added already
      isPaired = manageSlave();
    }
    else {
      // No slave found to process
    }
  
    // wait for 3seconds to run the logic again
    delay(3000);
  }

  // connected
  sendData(readAccelerometer());
  delay(50);
}
