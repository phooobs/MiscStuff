#include <SPI.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature temperatureSensor(&oneWire);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// state
enum States {LOGGING, WIFI_CONFIG};

// SPI commands
const byte SIMO_TEMPERATURE = B00000001;
const byte MISO_WIFI = B00000010;
const byte SIMO_WIFI = B00000011;

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
  static States state;
  
  switch(state) {
    case LOGGING :
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
      display.setTextSize(1);      // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.setCursor(0, 0);     // Start at top-left corner
      display.cp437(true);         // Use full 256 char 'Code Page 437' font
      display.print("Temperature: ");
      display.print(temperature);
      display.print(char(248)); // degree charicter
      display.print("C");
      display.display();
      break;
      
    case WIFI_CONFIG :
      state = TRANSFER_TEMPERATURE;
      break;

    default :
      state = TRANSFER_TEMPERATURE;
      break;
  }
}
