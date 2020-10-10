#include <SPI.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ONE_WIRE_BUS 2
#define UP_BUTTON 3
#define SELECT_BUTTON 4
#define DOWN_BUTTON 5

bool upButtonState = false;
bool lastUpButtonState = false;
bool selectButtonState = false;
bool lastSelectButtonState = false;
bool downButtonState = false;
bool lastDownButtonState = false;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature temperatureSensor(&oneWire);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// state
enum States {LOGGING, WIFI_CONFIG};

// SPI commands
const byte SIMO_TEMPERATURE = B00000001;
const byte SIMO_CONNECT_WIFI = B00000010;
const byte SIMO_ACCESS_POINT = B00000011;
const byte SIMO_PASSWORD = B00000100;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

float temperature = 0;



void setup() {
  Serial.begin(9600);
  
  digitalWrite(SS, HIGH); // disable Slave Select
  SPI.begin ();
  SPI.setClockDivider(SPI_CLOCK_DIV8);//divide the clock by 8

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println("SSD1306 allocation failed");
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();
}

void loop() {
  upButtonState = digitalRead(UP_BUTTON);
  selectButtonState = digitalRead(SELECT_BUTTON);
  downButtonState = digitalRead(DOWN_BUTTON);

  static States state;
  
  switch(state) {
    case LOGGING :
      state = logging();
      break;
      
    case WIFI_CONFIG :
      state = wifiConfig();
      break;

    default :
      state = LOGGING;
      break;
  }
  lastUpButtonState = upButtonState;
  lastSelectButtonState = selectButtonState;
  lastDownButtonState = downButtonState;
}

States logging () {

  if ((selectButtonState == true) && (lastSelectButtonState == false)) { // select button pressed switch to config menu
    return WIFI_CONFIG;
  }
  
  temperatureSensor.requestTemperatures();
  temperature = temperatureSensor.getTempCByIndex(0);

  // send temperature data to slave
  digitalWrite(SS, LOW); // enable Slave Select
  SPI.transfer(SIMO_TEMPERATURE);
  String sendData = String(temperature);
  for (unsigned int i = 0; i < sendData.length(); i++) {
    SPI.transfer(sendData[i]);
  }
  SPI.transfer('\r');
  digitalWrite(SS, HIGH); // disable Slave Select
      
  display.clearDisplay();
  display.drawRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(2, 2);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.print("Temperature: ");
  display.print(temperature);
  display.print(char(248)); // degree charicter
  display.print("C");
  display.display();

  return LOGGING;
}

States wifiConfig () {
  enum MenuItem {BACK, ACCESS_POINT, PASSWORD, APPLY, END};
  const String menuItemText[] = {"Back", "Set Access Point", "Set Password", "Apply"};
  static MenuItem menuPosition;

  if ((upButtonState == true) && (lastUpButtonState == false)) { // menu up
    if (menuPosition == 0) {
      menuPosition = END - 1;
    } else {
      menuPosition = menuPosition - 1;
    }
    
  } else if ((downButtonState == true) && (lastDownButtonState == false)) { // menu down
    if (menuPosition == END - 1) {
      menuPosition = 0;
    } else {
      menuPosition = menuPosition + 1;
    }
    
  } else if ((selectButtonState == true) && (lastSelectButtonState == false)) { // select button pressed
    switch(menuPosition) {
      case BACK :
        return LOGGING;
        break;
        
      case ACCESS_POINT : {
        Serial.println("Set Access Point");

        // send temperature data to slave
        digitalWrite(SS, LOW); // enable Slave Select
        SPI.transfer(SIMO_ACCESS_POINT);
        String sendData = "Guest";
        for (unsigned int i = 0; i < sendData.length(); i++) {
          SPI.transfer(sendData[i]);
        }
        SPI.transfer('\r');
        digitalWrite(SS, HIGH); // disable Slave Select
        break; }
  
      case PASSWORD : {
        Serial.println("Set Password");

        // send temperature data to slave
        digitalWrite(SS, LOW); // enable Slave Select
        SPI.transfer(SIMO_PASSWORD);
        String sendData = "";
        for (unsigned int i = 0; i < sendData.length(); i++) {
          SPI.transfer(sendData[i]);
        }
        SPI.transfer('\r');
        digitalWrite(SS, HIGH); // disable Slave Select
        break; }

      case APPLY : {
        Serial.println("Apply");
        // send temperature data to slave
        digitalWrite(SS, LOW); // enable Slave Select
        SPI.transfer(SIMO_CONNECT_WIFI);
        digitalWrite(SS, HIGH); // disable Slave Select
        break; }
    }
  }
  
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale

  
  
  for (MenuItem i = 0; i < END; i = i + 1) {
    display.setCursor(2, 2 + i * 10);     // Start at top-left corner
    if (menuPosition == i) {
      display.setTextColor(SSD1306_WHITE); // selected item, Draw Black text
      display.drawRect(0, i*10, SCREEN_WIDTH, 11, SSD1306_WHITE);
    } else {
      display.setTextColor(SSD1306_WHITE); // not selected item, Draw white text
    }
    display.print(menuItemText[i]);
  }
  
  display.display();

  return WIFI_CONFIG;
}
