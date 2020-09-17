#include <SoftwareSerial.h>

SoftwareSerial BlueSerial(2, 3); // RX, TX

const int dirPin = 4;
const int stepPin = 5;

int joystickValue = 0;
int joystickDeadzone = 50;
int joystickCenter = 511;

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
  // Read in bytes and convert to int with '\n' delimiter
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
  

  // Set Direction
  if (joystickValue - joystickCenter > joystickDeadzone) {
    digitalWrite(dirPin, HIGH);
    // Step
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2000);

    Serial.print("R ");
    Serial.println(joystickValue - joystickCenter);
    
  } else if (joystickValue - joystickCenter < -joystickDeadzone) {
    digitalWrite(dirPin, LOW);
    // Step
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2000);

    Serial.print("L ");
    Serial.println(joystickValue - joystickCenter);
  }

  
}
