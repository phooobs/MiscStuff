#include <SoftwareSerial.h>

const int joystickYPin = A7;
int joystickMaxValue = 0;

SoftwareSerial BlueSerial(2, 3); // RX, TX

void setup() {
  // set the data rate for the SoftwareSerial port
  BlueSerial.begin(9600);
}

void loop() {
  int joystickValue = analogRead(joystickYPin);
  if (joystickMaxValue < joystickValue) {
    joystickMaxValue = joystickValue;
  }
  BlueSerial.println(int(float(joystickValue) * 1023.0 / float(joystickMaxValue)));
}
