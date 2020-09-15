#include <SoftwareSerial.h>

const int joystickPin = A7;
const int CMDPin = A7;
SoftwareSerial BlueSerial(2, 3); // RX, TX

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  BlueSerial.begin(9600);
  while (!BlueSerial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  BlueSerial.println("AT+TID=2222222222");

  if (BlueSerial.available() > 0) {
    String incomingCommand = Serial.readString();
    Serial.println(incomingCommand);
  }
  
  BlueSerial.println("AT+RID=1111111111");
  
  if (BlueSerial.available() > 0) {
    String incomingCommand = Serial.readString();
    Serial.println(incomingCommand);
  }
}

void loop() {

  Serial.print(analogRead(joystickPin));
  if (BlueSerial.available() > 0) {
    String incomingCommand = Serial.readString();
    //Serial.print(" ");
    Serial.println(incomingCommand);
  }
}
