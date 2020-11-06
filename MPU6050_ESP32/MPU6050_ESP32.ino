#include <Wire.h>
const byte accelerometerAddress = 0x68;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // configure
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
  Wire.beginTransmission(accelerometerAddress);
  Wire.write(0x3B); // start at ACCEL_XOUT_H
  Wire.endTransmission(false);

  // request registers ACCEL_XOUT_H through ACCEL_ZOUT_L
  Wire.requestFrom(accelerometerAddress, 6, true); // request 6 bytes

  // read data into memory and scale to g's
  int16_t accelerometerXT, accelerometerYT, accelerometerZT;
  float accelerometerX, accelerometerY, accelerometerZ;
  if (Wire.available() == 6) {
    accelerometerXT = (Wire.read() << 8 | Wire.read());
    accelerometerX = accelerometerXT / 8192.0;
    accelerometerYT = (Wire.read() << 8 | Wire.read());
    accelerometerY = accelerometerYT / 8192.0;
    accelerometerZT = (Wire.read() << 8 | Wire.read());
    accelerometerZ = accelerometerZT / 8192.0;
  }
  Wire.endTransmission(true);

  // print data
  Serial.print(accelerometerX);
  Serial.print(" ");
  Serial.print(accelerometerY);
  Serial.print(" ");
  Serial.print(accelerometerZ);
  Serial.println();
}
