/**
 * Simple blinking
 */

#include <Arduino.h>
#include "base_functions.h"

#include <Wire.h>
#include "SSD1306Wire.h"

SSD1306Wire display(0x3c, SDA, SCL);

void updateScreen(String value) {
  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_CENTER); // coords define center of text
  display.drawString(64, 20, String(value));
  display.display();
}

void setup() {

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  DL("Hello there.");
  blink(3, 300);

  // connect_to_wifi();

  display.init();
  display.flipScreenVertically();
  updateScreen("Hello");

  display.drawHorizontalLine(10, 42, 108);
  display.display();
}

void loop() {
}


