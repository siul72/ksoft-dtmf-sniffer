#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMono9pt7b.h>

#define OLED_RESET 4
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

Adafruit_SSD1306 display(OLED_RESET);


#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

int timeNow = 0;
char* output;

void setup()   {
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  //display.setTextSize(1);
  display.setFont(&FreeMono9pt7b);



}


void loop() {

   // Storage for the message to be shown on the display
  char szDisp[128];

  // Read the data from the sensor
  float fTemp =  10;
  float fHum = 20;
  timeNow = timeNow +1 ;

  char szTemp[8];
  char szHum[8];
  dtostrf(fTemp, 1, 1, szTemp);
  dtostrf(fHum, 1, 1, szHum);

  int m = timeNow / 60;
  int h = m / 60;
  m = m % 60;
  int s = timeNow % 60;


  //sprintf(szDisp, "U: %02d:%02d:%02d\nT: %s C\nH: %s%%", h,m,s, szTemp, szHum);
  sprintf(szDisp, "T: %d\nU: %02d:%02d:%02d\n", timeNow, h,m,s );

   // Show the computed value
  display.clearDisplay();
  display.setCursor(0, 20);
  display.println(szDisp);
  display.display();

  // Values can only be read from the sensor every two seconds
  delay(1000);

}
