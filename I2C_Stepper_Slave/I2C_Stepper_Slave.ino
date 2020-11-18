#include<Wire.h>
#include <Stepper.h>

const int stepsPerRevolution = 2038;

Stepper stepper(stepsPerRevolution, 13, 11, 12, 10); // 8, 10, 9, 11

int8_t target;

void receiveEvent(int bytes){
  for (int i = 0; i < bytes; i++) { // get all the bytes only use the most up to date one
    target = Wire.read();
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin(9);// matching the address defined in the master. 
  Wire.onReceive(receiveEvent);// whenever receives anything will trigger the receiveEvent() function
  stepper.setSpeed(5);
}

void loop() {
  // rotate twords the target
  // each increment of the target is 18 degrees
  // each increment of the stepper is 0.1766437684 degrees
  static float position;
  if (position + 0.2 < target * 18) {
    stepper.step(1);
    position +=  0.1766437684;

    // print status
    Serial.print(target * 18);
    Serial.print(" ");
    Serial.println(position);
  } else if (position - 0.2  > target * 18) {
    stepper.step(-1);
    position -= 0.1766437684;

    // print status
    Serial.print(target * 18);
    Serial.print(" ");
    Serial.println(position);
  }
}
