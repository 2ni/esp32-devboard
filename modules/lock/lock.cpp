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

#include "base_functions.h"
#include "SSD1306Wire.h"
#include "fonts.h"

#define SLOT_FRAME_WIDTH 25
#define SLOT_FRAME_HEIGHT 31
#define SLOT_FRAME_GAP 5
#define SLOT_FRAME_X 21
#define SLOT_FRAME_Y 24

#define STATUS_X 0
#define STATUS_Y 0

SSD1306Wire display(0x3c, SDA, SCL);

Rotary rotary = Rotary(ROTARY_A, ROTARY_B);

String code = "007";
String curCode = "000";

unsigned int lastSlot = 0;
volatile unsigned int curSlot = 0;

unsigned int digitNeedsUpdate = false;
unsigned int isAdmin = false;
volatile unsigned int rotaryResult;

int lastPeriod = 0;
volatile int curPeriod = 1;

String curDigit = "0";
String curStatus = "LOCKED";

hw_timer_t *timer = NULL;

int getSlotX(int index) {
  return SLOT_FRAME_X + index * (SLOT_FRAME_WIDTH + SLOT_FRAME_GAP);
}

void drawSlotFrame(int x, int y) {
  display.drawRect(x, y, SLOT_FRAME_WIDTH, SLOT_FRAME_HEIGHT);
  display.display();
}

void clearSlotFrame(int x, int y) {
  display.setColor(BLACK);
  drawSlotFrame(x, y);
  display.setColor(WHITE);
}

void drawDigit(int x, int y, String digit) {
  display.setFont(Unibody8Pro_Regular_24);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(x + 2, y - 4, digit);
  display.display();
  curDigit = digit;
}

void clearDigit(int x, int y) {
  display.setColor(BLACK);
  drawDigit(x, y, curDigit);
  display.setColor(WHITE);
}

void drawCodeInput(int x, int y) {
  int y2 = y;
  int x2 = x;
  for (int i = 0; i < 3; i++) {
    drawSlotFrame(x2, y2);
    drawDigit(x2, y2, String(curCode[i]));
    x2 += SLOT_FRAME_WIDTH + SLOT_FRAME_GAP;
  }
}

void updateDigit() {
  digitNeedsUpdate = false;
  int slotX = getSlotX(curSlot);
  clearDigit(slotX, SLOT_FRAME_Y);
  drawDigit(slotX, SLOT_FRAME_Y, String(curCode[curSlot]));
}

void drawStatus(int x, int y, String text) {
  display.setFont(Unibody8Pro_Regular_8);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(x, y, text);
  display.display();
  curStatus = text;
}

void clearStatus(int x, int y, String text) {
  display.setColor(BLACK);
  drawStatus(x, y, text);
  display.setColor(WHITE);
}

void setStatus(String text) {
  clearStatus(STATUS_X, STATUS_Y, curStatus);
  drawStatus(STATUS_X, STATUS_Y, text);
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

  drawCodeInput(SLOT_FRAME_X, SLOT_FRAME_Y);

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
  unsigned int slotX;

  if (curPeriod != lastPeriod) {
    slotX = getSlotX(curSlot);
    if (curPeriod) {
      clearSlotFrame(slotX, SLOT_FRAME_Y);
    } else {
      drawSlotFrame(slotX, SLOT_FRAME_Y);
    }

    lastPeriod = curPeriod;
  }

  if (curSlot != lastSlot) {
    slotX = getSlotX(lastSlot);
    drawSlotFrame(slotX, SLOT_FRAME_Y);

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

  if (digitNeedsUpdate) {
    updateDigit();
  }
}
