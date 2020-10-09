#include <SPI.h>
volatile boolean process;
volatile float temperature;

// commands
const byte SIMO_TEMPERATURE = B00000001;
const byte MISO_WIFI = B00000010;
const byte SIMO_WIFI = B00000011;

String AP = "Guest";       // AP NAME. Change it to the AP you are using. You can even use the your smartphone's hotspot. 
String PASS = ""; // AP PASSWORD. At FLC, the Guest AP doesn't need a password
String API = "GVDN5WA12ZF3ZCU3"; // Must be your ThingSpeak channel's API. 
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 


 
void setup (void) {
  // ESP8266 setup
  
  Serial.begin(115200); // ESP setup 
  sendCommand("AT+RST",5,"OK");
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  String cmd = F("AT+CWJAP=\"");
  cmd += AP;
  cmd += F("\",\"");
  cmd += PASS;
  cmd += F("\"");
  sendCommand(cmd,20,"OK");


  pinMode(MISO, OUTPUT); // have to send on master in so it set as output
  SPCR |= _BV(SPE); // turn on SPI in slave mode
  process = false;
  SPI.attachInterrupt(); // turn on interrupt
}
 
ISR (SPI_STC_vect) { // SPI interrupt routine

  static char buff [50];
  static byte indx;
  
  static byte curentCommand;
  byte recivedByte = SPDR; // read byte from SPI Data Register
  
  if (curentCommand == SIMO_TEMPERATURE) {
    if (indx < sizeof buff) {
      buff [indx++] = recivedByte; // save data in the next index in the array buff
      if (recivedByte == '\r') { //check for the end of the word
        temperature = atof(buff);
        process = true;
        indx = 0;
        curentCommand = B00000000;
      }
    }
  } else if (curentCommand == MISO_WIFI) {

    curentCommand = B00000000;
  } else if (curentCommand == SIMO_WIFI) {

    curentCommand = B00000000;
  } else { // we dont have a curent command, the recived byte is the command
    curentCommand = recivedByte;
  }
}
 
void loop (void) {
  if (process) {
    process = false; //reset the process

    String sensorByteStr = String(temperature);  
    String getData = "GET /update?api_key="+ API +"&field1="+sensorByteStr; // send to ThingSpeak, needs to be a string
    sendCommand("AT+CIPMUX=1",5,"OK");
    sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
    sendCommand("AT+CIPSEND=0," + String(getData.length()+4),4,">");
    Serial.println(getData);
    sendCommand("AT+CIPCLOSE=0",5,"OK");
  }
}

void sendCommand(String command, int maxTime, char readReplay[]){ // send data to ESP8266
  while(countTimeCommand < (maxTime*1)) {
    Serial.println(command); //at+cipsend
    if(Serial.find(readReplay)) { //ok
      found = true;
      break;
    }  
    countTimeCommand++;
  }  
  if(found == true) {
    countTrueCommand++;
    countTimeCommand = 0;
  }  
  if(found == false) {
    countTrueCommand = 0;
    countTimeCommand = 0;
  }  
  found = false;
}
