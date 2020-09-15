const int analogPin = A0;
 
void setup() {
  //Setup serial connection
  Serial.begin(115200); 
}
 
void loop() {                                                 
  Serial.write(int((float(analogRead(analogPin)) / 1024.0) * 255.0));
  delay(1);                                    
}
