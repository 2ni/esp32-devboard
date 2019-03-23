/**
 * Simple blinking by pushing the button or rotary
 *
 */

#include <Arduino.h>
#include "base_functions.h"

#include <Rotary.h> // git clone https://github.com/buxtronix/arduino/blob/master/libraries/Rotary into lib/

Rotary rotary = Rotary(ROTARY_A, ROTARY_B);
int counter = 0;

hw_timer_t * timer = NULL;
hw_timer_t * timerPush = NULL;

typedef enum {
  NO_PRESS,
  LONG_PRESS,
  SHORT_PRESS,
} pushMode;

pushMode rotaryMode = NO_PRESS;

void IRAM_ATTR resetLED() {
  digitalWrite(LED, 0);
}

void IRAM_ATTR handleLED(int us) {
  digitalWrite(LED, 1);
  timer = timerBegin(0, 80, true); // 1 tick = 1us with pre divider of 80MHz
  timerAttachInterrupt(timer, &resetLED, true);
  timerAlarmWrite(timer, us, false);
  timerAlarmEnable(timer);
}

void IRAM_ATTR handleLED() {
  handleLED(1e5);
}

void IRAM_ATTR buttonPushed() {
  handleLED();
  DL("Button pushed");
}

void IRAM_ATTR longPress() {
  rotaryMode = LONG_PRESS;
  DL("long");
}

/*
 * short press on release
 * long press on timeout
 */
void IRAM_ATTR rotaryPushed() {
  int v = digitalRead(ROTARY_BUTTON);
  // push (RISING)
  if (v) {
    rotaryMode = NO_PRESS;
    handleLED();

    // start timer for potential long push
    timerPush = timerBegin(1, 80, true);
    timerAttachInterrupt(timerPush, &longPress, true);
    timerAlarmWrite(timerPush, 5e5, false); // 500ms timeout for long press
    timerAlarmEnable(timerPush);

  // release (FALLING)
  } else {
    timerAlarmDisable(timerPush); // long push was not reached
    // timerWrite(timerPush, 0); // reset timer
    if (rotaryMode != LONG_PRESS) {
      rotaryMode = SHORT_PRESS;
      DL("short");
    }
  }
}

void rotate() {
  unsigned char result = rotary.process();
  if (result == DIR_CW) {
    counter++;
    DF("counter: %d\n", counter);
    handleLED();
  } else if (result == DIR_CCW) {
    counter--;
    DF("counter: %d\n", counter);
    handleLED();
  }
}


void setup() {

  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(ROTARY_BUTTON, INPUT);

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  DL("Hello there.");
  blink(1, 100);

  // connect_to_wifi();

  attachInterrupt(digitalPinToInterrupt(BUTTON), buttonPushed, RISING);
  attachInterrupt(digitalPinToInterrupt(ROTARY_BUTTON), rotaryPushed, CHANGE);

  attachInterrupt(digitalPinToInterrupt(ROTARY_A), rotate, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_B), rotate, CHANGE);

}

void loop() {
}


