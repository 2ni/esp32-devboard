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

void drawPoint(int8_t x, int8_t y) {
  display.fillRect(x, y+1, 6, 4);
  display.fillRect(x+1, y, 4, 6);
}

void setPeriod(int8_t position) {
  int8_t row, col;
  col = position%13;
  row = ceil(position/13);
  drawPoint(col*10+1, row*10);
}

void clearPeriod(int8_t position) {
  display.setColor(BLACK);
  setPeriod(position);
  DF("clearing: %u\n", position);
  display.setColor(WHITE);
}

hw_timer_t * timer = NULL;
int8_t lastPeriod = 0;
volatile int8_t curPeriod = 1;
void IRAM_ATTR togglePeriod() {
  curPeriod = !curPeriod;
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

  for(int8_t i=0; i<42; i++) {
    setPeriod(i);
  }

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &togglePeriod, true);
  timerAlarmWrite(timer, 1e6, true);
  timerAlarmEnable(timer);

}

void loop() {
  if (curPeriod != lastPeriod) {
    if (curPeriod) {
      clearPeriod(41);
    } else {
      setPeriod(41);
    }
    display.display();
    lastPeriod = curPeriod;
  }
}


