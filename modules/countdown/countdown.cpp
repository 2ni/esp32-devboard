/**
 * Simple countdown shown on SSD1306 display
 *
 * Playing RTTL melodies
 *
 * Dependencies:
 *   - ESP8266_SSD1306_ID562
 *
 * pio lib install 562
 */

#include <Arduino.h>
#include "base_functions.h"

#include <Wire.h>
#include "SSD1306Wire.h"
#include "fonts.h"

#include "Rotary.h"

SSD1306Wire display(0x3c, SDA, SCL);
/*
Rotary rotary = Rotary(ROTARY_A, ROTARY_B);
int counter = 0;
void rotate() {
  unsigned char result = rotary.process();
  if (result == DIR_CW) {
    counter++;
    DF("counter: %d\n", counter);
  } else if (result == DIR_CCW) {
    counter--;
    DF("counter: %d\n", counter);
  }
}
*/

void updateTime(String value) {
  display.setFont(Unibody8Pro_Regular_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT); // coords define center of text
  display.drawString(1, 54, String(value));
  display.display();
}

void clearTime(String value) {
  display.setColor(BLACK);
  updateTime(value);
  display.setColor(WHITE);
}

void drawPoint(int8_t x, int8_t y) {
  display.fillRect(x, y+1, 6, 4);
  display.fillRect(x+1, y, 4, 6);
  display.display();
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
  display.setColor(WHITE);
}

void sec2human(const uint16_t seconds, char *str) {
  uint16_t h, m, s;
  uint16_t t = seconds;
  s = t % 60;
  t = (t-s)/60;
  m = t % 60;
  t = (t-m)/60;
  h = t;
  sprintf(str, "%02d:%02d:%02d", h, m, s);
}

hw_timer_t * timer = NULL;
char countdownString[8] = "";
char oldCountdownString[8] = "";
volatile uint16_t countdown;
volatile int8_t periodVisible, lastPeriodVisible;
uint8_t period = 0;
uint8_t lastPeriod = 0;

void IRAM_ATTR togglePeriod() {
  periodVisible = !periodVisible;
  countdown--;
}

void setup() {
  pinMode(LED, OUTPUT);

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  DL("Hello there.");
  blink(1, 100);

  // connect_to_wifi();
  countdown = 10;
  period = ceil(countdown/60);
  lastPeriod = period;
  periodVisible = !((countdown/2)%2); // even seconds means show period
  lastPeriodVisible = !periodVisible;

  // attachInterrupt(digitalPinToInterrupt(ROTARY_A), rotate, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(ROTARY_B), rotate, CHANGE);

  display.init();
  display.flipScreenVertically();

  for(int8_t i=0; i<period; i++) {
    setPeriod(i);
  }

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &togglePeriod, true);
  timerAlarmWrite(timer, 1e6, true);
  timerAlarmEnable(timer);
}

void loop() {
  if (periodVisible != lastPeriodVisible) {
    sec2human(countdown, countdownString);
    clearTime(oldCountdownString);
    updateTime(countdownString);
    strncpy(oldCountdownString, countdownString, strlen(countdownString));

    period = ceil(countdown/60); // update num periods shown
    // DF("******* countdown: %u, period: %u, lastPeriod: %u\n", countdown, period, lastPeriod);

    if (period != lastPeriod) {
      clearPeriod(lastPeriod);
      lastPeriod = period;
      periodVisible = !periodVisible;
    }

    // toggle last period
    if (periodVisible) {
      clearPeriod(period);
    } else {
      setPeriod(period);
    }
    lastPeriodVisible = periodVisible;
  }

  if (!countdown) {
    timerAlarmDisable(timer);
    display.clear();
    display.setFont(Unibody8Pro_Regular_24);
    display.setTextAlignment(TEXT_ALIGN_CENTER); // coords define center of text
    display.drawString(64, 12, String("BOOOM!"));
    display.drawRect(0,0,128,64);
    display.display();
    while(1);
  }
}
