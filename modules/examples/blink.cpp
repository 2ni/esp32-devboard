/**
 * Simple blinking by pushing the button or rotary
 *
 */

#include <Arduino.h>
#include "base_functions.h"

#include <Rotary.h> // https://github.com/buxtronix/arduino/blob/master/libraries/Rotary
Rotary rotary = Rotary(ROTARY_A, ROTARY_B);
int counter = 0;

volatile int buttonState = 0;
hw_timer_t * timer = NULL;

void IRAM_ATTR resetLED() {
  digitalWrite(LED, 0);
}

void IRAM_ATTR handleLED() {
  digitalWrite(LED, 1);
  // 1 tick = 1us
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &resetLED, true);
  timerAlarmWrite(timer, 100000, false);
  timerAlarmEnable(timer);
}

void IRAM_ATTR buttonPushed() {
  // buttonState = digitalRead(BUTTON);
  handleLED();
  DL("Button pushed");
}

void IRAM_ATTR rotaryPushed() {
  // buttonState = digitalRead(ROTARY_BUTTON);
  handleLED();
  DL("Rotary button pushed");
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
  blink(3, 300);

  // connect_to_wifi();

  attachInterrupt(digitalPinToInterrupt(BUTTON), buttonPushed, RISING);
  attachInterrupt(digitalPinToInterrupt(ROTARY_BUTTON), rotaryPushed, RISING);

  attachInterrupt(digitalPinToInterrupt(ROTARY_A), rotate, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_B), rotate, CHANGE);

}

void loop() {
}


