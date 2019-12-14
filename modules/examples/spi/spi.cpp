/*
 * test spi with a loopback
 * connect miso - mosi
 *
 * test also rfm95 module
 *
 * inspired by
 *   https://circuitdigest.com/microcontroller-projects/arduino-spi-communication-tutorial
 *   https://arduino.stackexchange.com/questions/66942/spi-test-by-connecting-mosi-to-miso
 *
 */

#include <Arduino.h>
#include <SPI.h>
#include "base_functions.h"
#include "rfm95_registers.h"

#define RFM_RST IO2

const SPISettings settings(10E6, MSBFIRST, SPI_MODE0);

uint8_t rfm95_spi(uint8_t out) {
  uint8_t res = SPI.transfer(out);

  return res;
}

void rfm_nss(uint8_t val) {
  if (val) {
    SPI.beginTransaction(settings);
  } else {
    SPI.endTransaction();
  }
  digitalWrite(RFM_CS, val);
}

void writeReg(uint8_t addr, uint8_t data) {
  rfm_nss(0);
  rfm95_spi(addr | 0x80);
  rfm95_spi(data);
  rfm_nss(1);
}

uint8_t readReg(uint8_t addr) {
  rfm_nss(0);
  digitalWrite(RFM_CS, LOW);
  rfm95_spi(addr | 0x7f);
  uint8_t val = rfm95_spi(0x00);
  rfm_nss(1);
  return val;
}

void rfm95_init() {
  noInterrupts();
  digitalWrite(RFM_RST, 0);
  // delay(10);
  // pinMode(RFM_RST, INPUT); // set floating

  writeReg(RegOpMode, OPMODE_SLEEP);
  interrupts();
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(RFM_CS, OUTPUT);
  pinMode(RFM_RST, OUTPUT);

  digitalWrite(RFM_CS, HIGH);

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
  // SPI.setClockDivider(SPI_CLOCK_DIV8);

  rfm95_init();
}

void loop() {
  blink(1, 100);

  // test SPI
  // uint8_t received;
  // uint8_t x = random(255);
  // received = SPI.transfer(x);
  // DF("sent: %u, received: %u\n", x, received);

  // test RFM95
  DF("version: 0x%02X\n", readReg(RegVersion));

  delay(1000);
}
