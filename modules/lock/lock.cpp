/**
 * Simple lock for SSD1306 display with rotary button
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
SSD1306Wire display(0x3c, SDA, SCL);

#include <Rotary.h>

Rotary rotary = Rotary(ROTARY_A, ROTARY_B);

#define SLOT_FRAME_WIDTH 25
#define SLOT_FRAME_HEIGHT 31
#define SLOT_FRAME_GAP 5
#define SLOT_FRAME_X 21
#define SLOT_FRAME_Y 24

char code[4] = "007";
char curCode[4] = "000";

uint8_t lastSlot = 0;
volatile uint8_t curSlot = 0;

uint8_t digitNeedsUpdate = false;
uint8_t isAdmin = false;
volatile unsigned char rotaryResult;

int8_t lastPeriod = 0;
volatile int8_t curPeriod = 1;

hw_timer_t *timer = NULL;


int8_t getSlotX(int8_t index) {
  return SLOT_FRAME_X + index * (SLOT_FRAME_WIDTH + SLOT_FRAME_GAP);
}

void drawSlotFrame(int8_t x, int8_t y) {
  display.drawRect(x, y, SLOT_FRAME_WIDTH, SLOT_FRAME_HEIGHT);
  display.display();
}

void clearSlotFrame(int8_t x, int8_t y) {
  display.setColor(BLACK);
  drawSlotFrame(x, y);
  display.setColor(WHITE);
}

void drawDigit(int8_t x, int8_t y, String digit) {
  display.setFont(Unibody8Pro_Regular_24);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(x + 2, y - 4, String(digit));
  display.display();
}

void clearDigit(int8_t x, int8_t y) {
  display.setColor(BLACK);
  display.fillRect(x + 1, y + 1, SLOT_FRAME_WIDTH - 2, SLOT_FRAME_HEIGHT - 2);
  display.display();
  display.setColor(WHITE);
}

void drawCodeInput(int8_t x, int8_t y) {
  int8_t y2 = y;
  int8_t x2 = x;
  for (int8_t i = 0; i < 3; i++) {
    drawSlotFrame(x2, y2);
    drawDigit(x2, y2, String(curCode[i]));
    x2 += SLOT_FRAME_WIDTH + SLOT_FRAME_GAP;
  }
}

void updateDigit() {
  digitNeedsUpdate = false;
  int8_t slotX = getSlotX(curSlot);
  clearDigit(slotX, SLOT_FRAME_Y);
  drawDigit(slotX, SLOT_FRAME_Y, String(curCode[curSlot]));
}

void IRAM_ATTR rotaryPushed() {
  curSlot += 1;
}

void IRAM_ATTR rotaryRotate() {
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
  while(!Serial) { }

  DL("Hello there.");
  blink(1, 100);

  // connect_to_wifi();

  display.init();
  display.flipScreenVertically();

  drawCodeInput(SLOT_FRAME_X, SLOT_FRAME_Y);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &togglePeriod, true);
  timerAlarmWrite(timer, 5e5, true);
  timerAlarmEnable(timer);

  attachInterrupt(digitalPinToInterrupt(ROTARY_BUTTON), rotaryPushed, RISING);

  attachInterrupt(digitalPinToInterrupt(ROTARY_A), rotaryRotate, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_B), rotaryRotate, CHANGE);
}

void loop() {
  uint8_t slotX;

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
      isAdmin = (strcmp(curCode, code) == 0);
      DF("Is admin: %s %s %d\n", curCode, code, isAdmin);
      curSlot = 0;
    }

    digitNeedsUpdate = true;
    lastSlot = curSlot;
  }

  if (rotaryResult) {
    uint8_t nr = (int)curCode[curSlot] - 48;

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
    DF("counter: %s\n", curCode);
    digitNeedsUpdate = true;
    rotaryResult = 0;
  }

  if (digitNeedsUpdate) {
    updateDigit();
  }
}
