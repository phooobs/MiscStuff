class HCSR04 {
  public:
  HCSR04 (int trig, int echo) {
    trig_ = trig;
    echo_ = echo;
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
  }

  float measure() {
    digitalWrite(trig_, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_, LOW);
    
    return((pulseIn(echo_, HIGH)) / 58.0);
  }
  
  private:
  int trig_;
  int echo_;
};

HCSR04 distanceSensor(2, 3);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(distanceSensor.measure());
  delay(100);
}
