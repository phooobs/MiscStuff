#include<Wire.h>

const int ENCODER_CLK = 3;
const int ENCODER_DT = 2;

void setup() {
  Serial.begin(9600);
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  Wire.begin();
}

void loop() {
  static int8_t counter[3];
  static bool lastClock;
  bool clock = digitalRead(ENCODER_CLK);
  if (lastClock && !clock) { // did the encoder rotate?
    if (digitalRead(ENCODER_DT)) { // what was the direction of the rotation?
      counter[0]++;
    } else {
      counter[0]--;
    }
    
    Serial.println(counter[0]);

    Wire.beginTransmission(9); // send through I2C to slave, 9 here is the address of the slave board 
    Wire.write(counter[0]); // Transfers the value of potentiometer to the slave board            
    Wire.endTransmission(); 
  }
  lastClock = clock;
  delay(10);
}
