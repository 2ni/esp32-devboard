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
#include <Wire.h>

#include "SSD1306Wire.h"
#include "base_functions.h"
#include "fonts.h"
#include "base.h"

SSD1306Wire display(0x3c, SDA, SCL);

Rotary rotary = Rotary(ROTARY_A, ROTARY_B);

const short SLOT_FRAME_GAP = 5;
const short TOTAL_SLOTS = 3;

const Point STATUS_OFFSET = {0, 0};
const Point SLOT_FRAME_OFFSET = {21, 24};
const Dim SLOT_FRAME_DIM = {25, 31};

String code = "007";
String curCode = "000";

unsigned int lastSlot = 0;
volatile unsigned int curSlot = 0;

bool digitNeedsUpdate = false;
bool isAdmin = false;

volatile int curPeriod = 1;
int lastPeriod = 0;

String curDigit = "0";
String curStatus = "LOCKED";

volatile unsigned int rotaryResult;

Box slots[TOTAL_SLOTS];

hw_timer_t *timer = NULL;

void drawSlotFrame(Box slot) {
  display.drawRect(slot.x, slot.y, slot.width, slot.height);
  display.display();
}

void clearSlotFrame(Box slot) {
  display.setColor(BLACK);
  drawSlotFrame(slot);
  display.setColor(WHITE);
}

void drawDigit(Box slot, String digit) {
  display.setFont(Unibody8Pro_Regular_24);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(slot.x + 2, slot.y - 4, digit);
  display.display();
  curDigit = digit;
}

void clearDigit(Box slot) {
  display.setColor(BLACK);
  drawDigit(slot, curDigit);
  display.setColor(WHITE);
}

void drawCodeInput(Point p) {
  int x = p.x;
  for (int i = 0; i < TOTAL_SLOTS; i++) {
    Box slot = slots[i] = {x, p.y, SLOT_FRAME_DIM.width, SLOT_FRAME_DIM.height};
    drawSlotFrame(slot);
    drawDigit(slot, String(curCode[i]));
    x += SLOT_FRAME_DIM.width + SLOT_FRAME_GAP;
  }
}

void updateDigit() {
  digitNeedsUpdate = false;
  Box slot = slots[curSlot];
  clearDigit(slot);
  drawDigit(slot, String(curCode[curSlot]));
}

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

void IRAM_ATTR rotaryPushed() { curSlot += 1; }

void IRAM_ATTR rotaryRotate() { rotaryResult = rotary.process(); }

void IRAM_ATTR togglePeriod() { curPeriod = !curPeriod; }

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

  drawCodeInput(SLOT_FRAME_OFFSET);

  setStatus("LOCKED");

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &togglePeriod, true);
  timerAlarmWrite(timer, 5e5, true);
  timerAlarmEnable(timer);

  attachInterrupt(digitalPinToInterrupt(ROTARY_BUTTON), rotaryPushed, RISING);

  attachInterrupt(digitalPinToInterrupt(ROTARY_A), rotaryRotate, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_B), rotaryRotate, CHANGE);
}

void loop() {
  Box slot;

  // blink frame
  if (curPeriod != lastPeriod) {
    slot = slots[curSlot];
    if (curPeriod) {
      clearSlotFrame(slot);
    } else {
      drawSlotFrame(slot);
    }

    lastPeriod = curPeriod;
  }

  // check if code matches
  if (curSlot != lastSlot) {
    slot = slots[lastSlot];
    drawSlotFrame(slot);

    // check admin after last slot is filled
    if (curSlot > 2) {
      isAdmin = (curCode == code);

      setStatus(isAdmin ? "UNLOCKED" : "LOCKED");

      // DF("Is admin: %s %s %d\n", curCode, code, isAdmin);
      curSlot = 0;
    }

    digitNeedsUpdate = true;
    lastSlot = curSlot;
  }

  // handle rotary input
  if (rotaryResult) {
    unsigned int nr = (int)curCode[curSlot] - 48;

    if (rotaryResult == DIR_CW) {
      if (nr == 9) {
        nr = 0;
      } else {
        nr++;
      }
    } else if (rotaryResult == DIR_CCW) {
      if (nr == 0) {
        nr = 9;
      } else {
        nr--;
      }
    }

    curCode[curSlot] = (char)(nr + 48);
    // DF("counter: %s\n", curCode);
    digitNeedsUpdate = true;
    rotaryResult = 0;
  }

  // update display
  if (digitNeedsUpdate) {
    updateDigit();
  }
}
