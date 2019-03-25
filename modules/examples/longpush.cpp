/**
 * Simple blinking by pushing the button or rotary
 *
 * Dependency: git clone https://github.com/2ni/Button in lib/
 *
 */

#include <Arduino.h>
#include "base_functions.h"

#include <Rotary.h> // git clone https://github.com/buxtronix/arduino/blob/master/libraries/Rotary into lib/
#include <Button.h>

Rotary rotary = Rotary(ROTARY_A, ROTARY_B);
int counter = 0;

void buttonPressed(Button::pushType type) {
  if (type == Button::SHORT_PUSH) {
    DL("short push");
  } else if (type == Button::LONG_PUSH) {
    DL("long push");
  }
}

Button button = Button(ROTARY_BUTTON, LED, buttonPressed);

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

void setup() {

  pinMode(LED, OUTPUT);

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  DL("Hello there.");
  blink(1, 100);

  button.setup();

  // connect_to_wifi();

  attachInterrupt(digitalPinToInterrupt(ROTARY_A), rotate, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_B), rotate, CHANGE);
}

void loop() {
  button.read();
}
