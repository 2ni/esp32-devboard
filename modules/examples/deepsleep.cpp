/*
 * Simple deepsleep to test external rtc source
 * based on https://lastminuteengineers.com/esp32-deep-sleep-wakeup-sources/
 *
 * The following config is needed to use external crystal for RTC:
 * in ~/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/config/sdkconfig.h
 * #define CONFIG_ESP32_RTC_CLOCK_SOURCE_EXTERNAL_CRYSTAL 1
 *
 * instead of
 * #define CONFIG_ESP32_RTC_CLOCK_SOURCE_INTERNAL_RC 1
 *
 * measuring time:
 * use http://joeyh.name/code/moreutils/
 * (via https://unix.stackexchange.com/questions/104834/how-to-log-serial-data-with-time-stamp)
 *
 * brew install moreutils
 * make terminal | ts [%H.%M.%.S]
 * or
 * make terminal | ./ts
 *
 */

#include <Arduino.h>
#include "base_functions.h"

#define uS_TO_S_FACTOR 1000000  //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  10       //Time ESP32 will go to sleep (in seconds)

RTC_DATA_ATTR int bootCount = 0;

void wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  D("woke up by ");
  switch(wakeup_reason) {
    case 1  : DL("external signal using RTC_IO"); break;
    case 2  : DL("external signal using RTC_CNTL"); break;
    case 3  : DL("timer"); break;
    case 4  : DL("touchpad"); break;
    case 5  : DL("ULP program"); break;
    default : DL("deep sleep"); break;
  }
}

void setup() {
  pinMode(LED, OUTPUT);
  ++bootCount;

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  DL("Hello there.");
  blink(1, 100);

  DF("boot count: %u\n", bootCount);

  wakeup_reason();

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  DF("going to sleep for %usec\n", TIME_TO_SLEEP);
  DL("************************************************");
  esp_deep_sleep_start();
}

void loop() {
}


