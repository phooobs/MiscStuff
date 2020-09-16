#include <SoftwareSerial.h>

SoftwareSerial BlueSerial(2, 3); // RX, TX

int joystickValue = 0;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Native USB only
  }

  // set the data rate for the SoftwareSerial port
  BlueSerial.begin(9600);
}

void loop() {
  // read in bytes and convert to int with '\n' delimiter
  while (BlueSerial.available()) {
    char incomingByte = BlueSerial.read();
    static String incomingMessage;
    if (incomingByte == '\n') {
      joystickValue = incomingMessage.toInt();
      incomingMessage = "";
    } else {
      incomingMessage += incomingByte;
    }
  }

  // do stuff
  Serial.println(joystickValue);
}
