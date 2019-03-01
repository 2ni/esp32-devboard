/*
 * basic example to
 * get data from i2c moisture https://github.com/2ni/attiny84
 */

#include <Arduino.h>
#include "base_functions.h"
#include <Wire.h>

#define I2C_ADDR 0x21
#define I2C_GET_MOISTRAW 0x16
#define I2C_RESET        0x15
#define I2C_GET_RAW      0x14
#define I2C_GET_MOIST    0x13
#define I2C_SET_BLINK    0x12
#define I2C_GET_BLINK    0x11
#define I2C_GET_TEMP     0x10

/*
 * I2C functions
 */
void readraw(byte addr, byte reg, byte size) {
  Wire.beginTransmission(addr);
  Wire.write((uint8_t)reg);
  Wire.endTransmission();
  Wire.requestFrom(addr, size);
}

uint16_t read16(byte addr, byte reg) {
  uint16_t value;
  readraw(addr, reg, 2);

  byte in[2]={0};
  byte c=0;
  // h first, then l
  // ignore all except first 2 bytes
  while (Wire.available()) {
    byte b = Wire.read();
    if (c<2) {
      in[c] = b;
    }
    c++;
  }

  Wire.endTransmission();

  value = ((in[0] << 8)&0xff00) | in[1];
  return value;
}

/*
 * Read <size> uint16_t
 * and save them in values
 */
void read16(byte addr, byte reg, uint16_t *values, byte size) {
  byte c = 0;
  readraw(addr, reg, 2*size);

  while (Wire.available()) {
    if (c<size) {
      values[c] = Wire.read() << 8 | Wire.read();
    }
    c++;
  }
  Wire.endTransmission();
}

void setup() {
  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while (!Serial) { }

  DL("Hello there.");

  Wire.setClock(200000); // 400kHz might be too much
  Wire.begin(SDA, SCL);

  // check if slave is here
  Wire.beginTransmission(I2C_ADDR);
  byte err = Wire.endTransmission();
  if (err == 0) {
    DL("Yup. We're up and running");
  } else if (err == 4) {
    DL("Unknwon error at 0x%x!");
  }
}

void loop() {
  DF("temp: %d\n", (int16_t)read16(I2C_ADDR, I2C_GET_TEMP));

  uint16_t m = read16(I2C_ADDR, I2C_GET_MOIST);
  char cm[10];
  dtostrf((float)m/10, 0, 1, cm); // convert in % humidity 0-100 (given 0-1000)
  DF("moisture: %s%%\n", cm);

  delay(1000);
}

