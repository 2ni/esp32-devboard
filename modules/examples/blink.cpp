/**
 * Simple blinking by pushing the button or rotary
 *
 */

#include <Arduino.h>
// #include <WiFi.h>
#include "base_functions.h"

void setup() {
  /*
  WiFi.mode(WIFI_OFF);
  btStop();
  */
  pinMode(LED, OUTPUT);

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  DL("Hello there.");
  blink(1, 100);
}

void loop() {
  DL("blink")
  blink(2, 100);
  delay(1000);
}


