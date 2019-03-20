/**
 * Simple lock for SSD1306 display with rotary button
 *
 * Dependencies:
 *   - ESP8266_SSD1306_ID562
 *
 * pio lib install 562
 */

#include <Arduino.h>
#include <Rotary.h>
#include <SSD1306Wire.h>
#include <Wire.h>

#include "ViewManager.h"
#include "LockCode.h"
#include "base.h"
#include "base_functions.h"
#include "fonts.h"

SSD1306Wire display(0x3c, SDA, SCL);

Rotary rotary = Rotary(ROTARY_A, ROTARY_B);

const Point STATUS_OFFSET = {0, 0};

bool isAdmin = false;

volatile int curPeriod = 1;
int lastPeriod = 0;

String curStatus = "LOCKED";

volatile unsigned int rotaryPushed;
volatile unsigned int rotaryResult;

ViewManager viewManager = ViewManager(&display);
LockCode lockCode = LockCode(&display);

hw_timer_t *timer = NULL;

void drawStatus(Point p, String text) {
  display.setFont(Unibody8Pro_Regular_8);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(p.x, p.y, text);
  display.display();
  curStatus = text;
}

void clearStatus(Point p, String text) {
  display.setColor(BLACK);
  drawStatus(p, text);
  display.setColor(WHITE);
}

void setStatus(String text) {
  Point p = STATUS_OFFSET;
  clearStatus(p, curStatus);
  drawStatus(p, text);
}

void IRAM_ATTR onRotaryPushed() {
  rotaryPushed = true;
}
void IRAM_ATTR onRotaryRotate() {
  rotaryResult = rotary.process();
}

void IRAM_ATTR togglePeriod() {
  curPeriod = !curPeriod;
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(ROTARY_BUTTON, INPUT);

  pinMode(I2S_EN, OUTPUT);
  digitalWrite(I2S_EN, 1); // activate I2S

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while (!Serial) {
  }

  DL("Hello there.");
  blink(1, 100);

  // connect_to_wifi();

  display.init();
  display.flipScreenVertically();

  lockCode.setup();

  setStatus("LOCKED");

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &togglePeriod, true);
  timerAlarmWrite(timer, 5e5, true);
  timerAlarmEnable(timer);

  attachInterrupt(digitalPinToInterrupt(ROTARY_BUTTON), onRotaryPushed, RISING);

  attachInterrupt(digitalPinToInterrupt(ROTARY_A), onRotaryRotate, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_B), onRotaryRotate, CHANGE);
}

void loop() {
  // blink frame
  if (curPeriod != lastPeriod) {
    if (curPeriod) {
      lockCode.clearCurrentSlotFrame();
    } else {
      lockCode.drawCurrentSlotFrame();
    }

    lastPeriod = curPeriod;
  }

  if (rotaryPushed) {
    if (lockCode.isLastSlot()) {
      isAdmin = lockCode.checkCode();
      setStatus(isAdmin ? "UNLOCKED" : "LOCKED");
      if (!isAdmin) {
        lockCode.selectSlot(0);
      }
    } else {
      lockCode.nextSlot();
    }

    rotaryPushed = false;
  }

  // handle rotary input
  if (rotaryResult) {
    if (rotaryResult == DIR_CW) {
      lockCode.countUp();
    } else if (rotaryResult == DIR_CCW) {
      lockCode.countDown();
    }

    rotaryResult = 0;
  }
}
