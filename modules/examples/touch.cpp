/**
 * Simple touch example on pin "IO2" (GPIO4)
 */

#include <Arduino.h>
#include "base_functions.h"


int threshold = 0;
byte released = 1;

void setup() {

  pinMode(LED, OUTPUT);

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  DL("Hello there.");
  blink(3, 300);

  for(int i=0; i<100; i++) {
    threshold += touchRead(IO2);
  }
  threshold /= 100;
  threshold *= .9;
  DF("threshold: %d\n", threshold);

  // connect_to_wifi();
}

void loop() {
  // DF("value: %d\n", touchRead(IO2));

  int value = 0;
  for (int i=0; i<100; i++) {
    value += touchRead(IO2);
  }
  value /= 100;

  if (released && value < threshold) {
    DL("switching LED");
    digitalWrite(LED, !digitalRead(LED));
    released = 0;
  }

  if (!released && value > threshold) {
    released = 1;
  }

  delay(100);
}


