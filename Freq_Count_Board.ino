// Frequency Counter Board
// IMSAI Guy 2021

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <FreqCount.h>
#include <EEPROM.h>

// I2C addresses
#define OLED_ADDR   0x3C

Adafruit_SSD1306 display(-1);

#define button1 A0  
#define button2 A1
#define div64256 2                    // HIGH = 64  LOW = 256

double frequency  = 123.456;          // power on frequency
double uncalfreq = 123.456;
float cal = 1.00000f;                 // calibration error
float readcal = 0.000f;               // used to read cal value in EEPROM
unsigned long count = 0;              // counter value
int mode = 1;                         // instrument mode
char displayString[17] = "";
int divisor = 256;                    // set to 64 or 256

//========================================================================
void setup() {
  
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(div64256, OUTPUT);
  if(divisor == 64) {
    digitalWrite(div64256,HIGH);
    }
  else {
    digitalWrite(div64256,LOW); 
    }
  
  // initialize and clear display
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,15);
  display.setFont(&FreeSansBold9pt7b);
  display.println("ImasiGuy");          // Splash
  display.setFont();
  display.display();
  delay(2000);

  EEint();                              // get stored cal data
  FreqCount.begin(1000);                // start counter
}
//========================================================================
void loop() {
  if (FreqCount.available()) {
      count = FreqCount.read();
      count = count * divisor;
      uncalfreq = count / 1000000.0;
      frequency = uncalfreq * cal;
      }
  if (mode == 1) updatedisplay();    
  if (mode == 2) calibration();
      
  if (digitalRead(button1) == 0) mode = 1;
  if (digitalRead(button2) == 0) mode = 2;
    
}
//========================================================================
void updatedisplay(){
    display.clearDisplay();
    display.setCursor(4,0);
    display.println("Frequency Counter");
    display.println();
    display.setFont(&FreeSansBold9pt7b);
    dtostrf(frequency, 7, 3, displayString);
    display.print(displayString);
    display.println(" MHz");
    display.setFont();
    display.display();
    delay(1000);
}
void calibration(){
    display.clearDisplay();
    display.setCursor(0,0);
    display.setFont();
    display.println("Calibration");
    display.println("Connect 10 MHz");
    display.println("Then Press SW 2");
    display.display();
    delay(1000);
    while(digitalRead(button2) == 1) {} // wait until key press
    cal = 10.000 / uncalfreq;           // calculate error, new cal factor
    EEPROM.put(1,cal);                  // store it
    display.clearDisplay();
    display.setCursor(0,0);
    display.setFont(&FreeSansBold9pt7b);
    display.println("Cal Complete");
    display.setFont();
    display.display();
    mode = 1;                           // exit mode 2
    delay(2000);
} 

//========================================================================
void EEint(){
  byte value = EEPROM.read(0);             // check flag
  if (value == 0x55) {                     // already initialized
     EEPROM.get(1, readcal);               // read EEPROM data
     cal = readcal;                        
     display.clearDisplay();
     display.setCursor(0,0);
     display.println("Cal data found");
     display.print("Cal Value: ");
     display.println(cal);
     display.display();
     delay(2000);
     }
  else {                                   // first time use, initialize EEPROM
    EEPROM.write(0, 0x55);                 // write flag
    EEPROM.put(1,cal);                     // write initial cal value
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("First time use");
    display.println("EEPROM initialized");
    display.display();
    delay(2000);      
    }
 }

//==============================================================
//==============================================================
