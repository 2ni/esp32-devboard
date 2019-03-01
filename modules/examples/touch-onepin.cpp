/**
 * Simple touch example on pin "IO2" (GPIO4)
 * based on https://www.youtube.com/watch?v=BO3umH4Ht8o&t=547s
 * and https://www.instructables.com/id/Turn-a-pencil-drawing-into-a-capacitive-sensor-for/
 *
 * set 1M resistance between 3.3v and IO2
 * attach touch surface to IO2
 *
 * Doesn't work very well, as 1M very high (compared to input impedance it seems)
 */

#include <Arduino.h>
#include "base_functions.h"

#include "xtensa/core-macros.h"

volatile uint32_t tr = 0;
hw_timer_t * timer = NULL;
uint32_t counter ;
byte waitForInterrupt = 0;

// isr when touch pin rises
void IRAM_ATTR inputIsr() {
  cli();
  tr = XTHAL_GET_CCOUNT() - counter;
  waitForInterrupt = 0;
  detachInterrupt(digitalPinToInterrupt(IO2));
  sei();
}

void setup() {
  pinMode(LED, OUTPUT);

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  DL("Hello there.");
  blink(3, 300);
}

void loop() {
  waitForInterrupt = 1;
  pinMode(IO2, OUTPUT); // discharge cap by setting output and low
  digitalWrite(IO2, LOW);
  delay(100);
  DF("value: %u\n", tr);
  pinMode(IO2, INPUT); // make an input without internal pull-up
  attachInterrupt(digitalPinToInterrupt(IO2), inputIsr, RISING); // wait until cap charged
  counter = XTHAL_GET_CCOUNT();
  while(true) {
    delay(1);
    if (waitForInterrupt == 0) {
      break;
    }
  }
}
