#include <SoftwareSerial.h>

const int joystickPin = A7;

SoftwareSerial BlueSerial(2, 3); // RX, TX

void setup() {
  // set the data rate for the SoftwareSerial port
  BlueSerial.begin(9600);
}

void loop() {
    BlueSerial.println(analogRead(joystickPin));
}
