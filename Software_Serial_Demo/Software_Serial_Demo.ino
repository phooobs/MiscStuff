#include <SoftwareSerial.h>

SoftwareSerial softSerial(2, 3); // RX, TX

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  softSerial.begin(9600);
}

void loop() {
  if (softSerial.available())
    Serial.write(softSerial.read());
  if (Serial.available())
    softSerial.write(Serial.read());
}

