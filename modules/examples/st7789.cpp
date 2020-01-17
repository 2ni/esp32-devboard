/**
 * testing lcd ST7789
 * based on https://github.com/ananevilya/Arduino-ST7789-Library
 *
 * Aliexpress ST7789 w/o CS pins only work with SPI_MODE3
 * changes needed in Arduino_ST7789.cpp:
 *   15: #include "base_functions.h"
 *   244: SPI.begin(SCK, MISO, MOSI);
 *   245: mySPISettings = SPISettings(24000000, MSBFIRST, SPI_MODE3);
 *
 */

#include <Arduino.h>
#include "base_functions.h"

#include <Adafruit_GFX.h>    // Core graphics library by Adafruit
#include <Arduino_ST7789.h> // Hardware-specific library for ST7789 (with or without CS pin)
#include <SPI.h>

#define RST CS0 // reset (works as cs as well, as no cs available)
#define BLK IO1 // back light
#define DC IO2  // data/command

// Arduino_ST7789 tft = Arduino_ST7789(DC, RST, MOSI, SCK);
Arduino_ST7789 tft = Arduino_ST7789(DC, RST); // using hw spi doesn't seem to work

float p = 3.1415926;

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void setup() {

  pinMode(LED, OUTPUT);
  pinMode(BLK, OUTPUT);

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  DL("Hello there.");
  blink(1, 100);

  tft.init(240, 240);

  tft.fillScreen(BLACK);

  // turn on screen
  digitalWrite(BLK, 1);

  DL("draw rect with cross");

  uint16_t colors[7] = { YELLOW, GREEN, BLUE, CYAN, MAGENTA, WHITE, RED };
  uint8_t rect_width = 100;
  uint8_t cross_width = 80;    // should always be even
  uint8_t cross_thickness = 11; // should always be odd

  for (uint8_t c=0; c<7; c++) {
    // rect
    tft.fillRect(tft.width()/2-rect_width/2, tft.height()/2-rect_width/2, rect_width, rect_width, colors[c]);

    // cross
    uint8_t count_width = cross_width/2;
    uint8_t count_thickness = (cross_thickness-1)/2;
    for (int8_t i=-count_width; i<=count_width; i++) {
      for (int8_t t=-count_thickness; t<=count_thickness; t++) {
        tft.drawPixel(tft.width()/2+i, tft.height()/2, BLACK);
        tft.drawPixel(tft.width()/2+i, tft.height()/2+t, BLACK);

        tft.drawPixel(tft.width()/2, tft.height()/2+i, BLACK);
        tft.drawPixel(tft.width()/2+t, tft.height()/2+i, BLACK);
      }
    }
     delay(500);
  }
  DL("done");

  /*
  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", WHITE);
  delay(1000);

  tft.setTextWrap(false);
  tft.fillScreen(BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(GREEN);
  tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(WHITE);
  tft.print(" seconds.");
  delay(4000);
  */
}

void loop() {
}
