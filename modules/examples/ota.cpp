/**
 * Simple blinking by pushing the button or rotary
 *
 */

#include <Arduino.h>
#include <ArduinoOTA.h>
#include "SSD1306Wire.h"

#include "base_functions.h"

#define USE_DISPLAY

#ifdef USE_DISPLAY
SSD1306Wire display(0x3c, SDA, SCL);
#endif

void setup() {
  pinMode(LED, OUTPUT);

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  DL("Hello there.");
  blink(1, 100);

  connect_to_wifi();

  #ifdef USE_DISPLAY
  DL("use display");
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 20, String("OTA demo"));
  display.display();
  #endif

  // OTA
  // https://github.com/espressif/arduino-esp32/blob/master/libraries/ArduinoOTA/examples/BasicOTA/BasicOTA.ino
  // ArduinoOTA.setPort(3232);
  // ArduinoOTA.setHostname(nodename); // would also be default (aka username)
  // ArduinoOTA.setPassword(OTA_PASSWD);
  ArduinoOTA.onStart([]() {
    char type[10];
    if (ArduinoOTA.getCommand() == U_FLASH)
      sprintf(type, "sketch");
    else // U_SPIFFS
      sprintf(type, "filesystem");

    DF("Start updating %s with OTA\n", type);
    #ifdef USE_DISPLAY
    display.clear();
    char msg[16] = "Starting OTA";
    display.drawString(64, 20, msg);
    display.display();
    #endif
  });
  ArduinoOTA.onEnd([]() {
    #ifdef USE_DISPLAY
    display.clear();
    #endif
    DL("\nEnd OTA");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    DF("Progress: %u%%\r", (progress / (total / 100)));
    #ifdef USE_DISPLAY
    char msg[16];
    sprintf(msg, "Progress: %d", (progress/(total/100)));
    display.drawString(64, 20, msg);
    display.display();
    #endif
  });
  ArduinoOTA.onError([](ota_error_t error) {
    DF("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) { DL("Auth Failed"); }
    else if (error == OTA_BEGIN_ERROR) { DL("Begin Failed"); }
    else if (error == OTA_CONNECT_ERROR) { DL("Connect Failed"); }
    else if (error == OTA_RECEIVE_ERROR) { DL("Receive Failed"); }
    else if (error == OTA_END_ERROR) { DL("End Failed"); }
  });
  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
}


