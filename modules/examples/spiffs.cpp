/**
 * Simple example using SPIFF and saving/reading data
 *
 * depends on https://github.com/2ni/timy
 * git clone https://github.com/2ni/timy lib/
 *
 */

#include <Arduino.h>
#include <SPIFFS.h>
#include <timy.h>
#include "base_functions.h"

const char* FP = "/timestamp.dat";

void setup() {

  pinMode(LED, OUTPUT);

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  DL("Hello there.");
  // blink(3, 300);

  TIMY timy("pool.ntp.org");
  unsigned long timestamp = 0;
  char timestampHumanized[20] = "";

  // format SPIFFS if not yet done
  if (!SPIFFS.begin()) {
    DL("Formatting SPIFFS.");
    DL("Pls wait until formatting done, even if errors outputted!");
    if (!SPIFFS.begin(true)) {
      DL("Formatting SPIFFS failed!");
    }
    DL("********** Formatting done.");
  }

  // read & output last timestamp
  if (SPIFFS.exists(FP)) {
    File fp = SPIFFS.open(FP, FILE_READ);
    fp.read((uint8_t *)&timestamp, sizeof(timestamp));
    timy.ts2human(timestamp, timestampHumanized);
    DF("last timestamp: %s\n", timestampHumanized);
    fp.close();
  }

  // get current timestamp
  connect_to_wifi();
  timestamp = timy.get_local_timestamp();
  DF("timestamp: %lu\n", timestamp);
  timy.ts2human(timestamp, timestampHumanized);
  DF("humanized timestamp: %s\n", timestampHumanized);

  // write current timestamp to file
  File fp = SPIFFS.open(FP, FILE_WRITE);
  if (fp.write((uint8_t *)&timestamp, sizeof(timestamp))) {
    DL("timestamp written");
  }
  fp.close();
}

void loop() {
}
