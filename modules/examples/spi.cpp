/*
 * test spi with a loopback
 * connect miso - mosi
 *
 * inspired by
 *   https://circuitdigest.com/microcontroller-projects/arduino-spi-communication-tutorial
 *   https://arduino.stackexchange.com/questions/66942/spi-test-by-connecting-mosi-to-miso
 *
 */

#include <Arduino.h>
#include <SPI.h>
#include "base_functions.h"

byte x;

void setup() {
  pinMode(LED, OUTPUT);

  // test if pins are working as expected (can be set high)
  /*
  pinMode(SCK, OUTPUT);
  pinMode(MISO, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(IO1, OUTPUT);
  pinMode(IO2, OUTPUT);
  pinMode(CS0, OUTPUT);
  pinMode(I2S_DIN, OUTPUT);
  */

  /*
  digitalWrite(SCK, HIGH);
  digitalWrite(MISO, HIGH);
  digitalWrite(MOSI, HIGH);
  digitalWrite(IO1, HIGH);
  digitalWrite(IO2, HIGH);
  digitalWrite(CS0, HIGH);
  digitalWrite(I2S_DIN, HIGH);
  */

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  DL("Hello there.");

  SPI.begin(SCK, MISO, MOSI);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
}

void loop() {
  byte received;
  x = random(255);

  blink(1, 100);

  received = SPI.transfer(x);
  DF("sent: %u, received: %u\n", x, received);

  delay(1000);
}
