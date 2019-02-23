/*
* Scans ports for I2C device.
*/
#include <Arduino.h>
#include <Wire.h>

#include "base_functions.h"

void check_if_exist_I2C() {
  byte error, address;
  int nDevices;
  nDevices = 0;
  for (address = 1; address < 127; address++ )  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0){
      D("I2C device found at address 0x");
      if (address < 16)
        D("0");
      Serial.print(address, HEX);
      DL("  !");

      nDevices++;
    } else if (error == 4) {
      D("Unknow error at address 0x");
      if (address < 16)
        D("0");
      Serial.println(address, HEX);
    }
  } //for loop
  if (nDevices == 0) {
    DL("No I2C devices found");
  } else {
    DL("**********************************\n");
    //delay(1000);           // wait 1 seconds for next scan, did not find it necessary
  }
}

void scanPorts() {
  Wire.begin(SDA, SCL);
  check_if_exist_I2C();
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  pinMode(LED, OUTPUT);

  DL("");
  DL("Booting.");
  blink();

  //connect_to_wifi();

  DL("I2C Scanner to scan for devices");
}

void loop() {
  blink(1, 10);
  scanPorts();
  delay(1000);
}

