/*
 * simple ir proximity sensor
 * add a thick paper between ir sender and receiver module
 * with  duty_cycle = 30 (of 255) we get ~20cm distance
 *
 * SCK       : e14
 * MISO      : e12
 * MOSI      : e13
 * RFM_CS    : e18
 * RFM_INT   : e35
 */


#include <Arduino.h>
#include "base_functions.h"

#define PWM_CHANNEL 0
#define PWM_FRQ 38000
#define PWM_RESOLUTION 8

void setup() {
  pinMode(LED, OUTPUT);

   // enable ir
  pinMode(IR_RECEIVE_EN, OUTPUT);
  digitalWrite(IR_RECEIVE_EN, 1);

  pinMode(IR_SEND, OUTPUT);
  digitalWrite(IR_SEND, 0);
  pinMode(IR_RECEIVE, INPUT);

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  DL("Hello there.");
  blink(1, 100);

  ledcSetup(PWM_CHANNEL, PWM_FRQ, PWM_RESOLUTION); // pwm channel 0, 38kHz pwm, resolution 8bit
  ledcAttachPin(IR_SEND, PWM_CHANNEL);
  ledcWrite(PWM_CHANNEL, 30);
}

unsigned long now = millis();
uint32_t debounce_last = 0;
uint32_t debounce_delay = 100;
uint8_t signal_seen = 0;
uint8_t signal = 0;


void loop() {
  // 0 = we have an ir signal
  signal = !digitalRead(IR_RECEIVE);
  if ((millis() - debounce_last) > debounce_delay) {
    if (signal != signal_seen) {
      signal_seen = signal;
      if (signal) {
        DL("contact!");
      } else {
        DL("contact lost");
      }
      debounce_last = millis();
    }
  }
  /*
  if (millis() - now > 1000) {
    blink(1);
    DF("obstacle: %s\n", digitalRead(IR_RECEIVE) ? "no":"yes");
    now = millis();
  }
  */
}
