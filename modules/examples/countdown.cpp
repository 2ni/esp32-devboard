/**
 * Simple countdown shown on SSD1306 display
 *
 * Playing RTTL melodies
 *
 * Dependencies:
 *   - ESP8266_SSD1306_ID562
 *
 * pio lib install 562
 */

#include <Arduino.h>
#include "base_functions.h"

#include <Wire.h>
#include "SSD1306Wire.h"
#include "fonts.h"
SSD1306Wire display(0x3c, SDA, SCL);

/*
// depends on https://github.com/davidhrbaty/ArduBot/tree/master/Libraries/TimerFreeTone
//  https://github.com/mikalhart/Flash
//  adapt #include <pgmspace.h> in Flash.h
#include <pgmspace.h>
#include <Rtttl.h>
Rtttl Rtttl(LED);
*/

/*
#include "SPIFFS.h"
#include "SD.h"
#include "FS.h"
#include "HTTPClient.h"
#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorRTTTL.h"
#include "AudioOutputI2S.h"
*/

/*
const char * mario = "mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6";

const char rudolph[] PROGMEM =
"Rudolph the Red Nosed Raindeer:d=8,o=5,b=250:g,4a,g,4e,4c6,4a,2g.,g,a,g,a,4g,4c6,2b.,4p,f,4g,f,4d,4b,4a,2g.,g,a,g,a,4g,4a,2e.,4p,g,4a,a,4e,4c6,4a,2g.,g,a,g,a,4g,4c6,2b.,4p,f,4g,f,4d,4b,4a,2g.,g,a,g,a,4g,4d6,2c.6,4p,4a,4a,4c6,4a,4g,4e,2g,4d,4e,4g,4a,4b,4b,2b,4c6,4c6,4b,4a,4g,4f,2d,g,4a,g,4e,4c6,4a,2g.,g,a,g,a,4g,4c6,2b.,4p,f,4g,f,4d,4b,4a,2g.,4g,4a,4g,4a,2g,2d6,1c.6.";
// Plenty more at: http://mines.lumpylumpy.com/Electronics/Computers/Software/Cpp/MFC/RingTones.RTTTL
*/

/*
AudioGeneratorRTTTL *rtttl;
AudioFileSourcePROGMEM *file;
AudioOutputI2S *out;
*/

void updateTime(String value) {
  display.setFont(Unibody8Pro_Regular_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT); // coords define center of text
  display.drawString(1, 54, String(value));
  display.display();
}

void clearTime(String value) {
  display.setColor(BLACK);
  updateTime(value);
  display.setColor(WHITE);
}

void drawPoint(int8_t x, int8_t y) {
  display.fillRect(x, y+1, 6, 4);
  display.fillRect(x+1, y, 4, 6);
  display.display();
}

void setPeriod(int8_t position) {
  int8_t row, col;
  col = position%13;
  row = ceil(position/13);
  drawPoint(col*10+1, row*10);
}

void clearPeriod(int8_t position) {
  display.setColor(BLACK);
  setPeriod(position);
  DF("clearing: %u\n", position);
  display.setColor(WHITE);
}

void sec2human(const uint16_t seconds, char *str) {
  uint16_t h, m, s;
  uint16_t t = seconds;
  s = t % 60;
  t = (t-s)/60;
  m = t % 60;
  t = (t-m)/60;
  h = t;
  sprintf(str, "%02d:%02d:%02d", h, m, s);
}

hw_timer_t * timer = NULL;
int8_t lastPeriod = 0;
char countdownString[8] = "";
char oldCountdownString[8] = "";
volatile uint16_t countdown;
volatile int8_t curPeriod = 1;
void IRAM_ATTR togglePeriod() {
  curPeriod = !curPeriod;
  countdown--;
}


void setup() {

  pinMode(LED, OUTPUT);

  pinMode(I2S_EN, OUTPUT);
  digitalWrite(I2S_EN, 1); // activate I2S

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  DL("Hello there.");
  blink(1, 100);

  // connect_to_wifi();

  display.init();
  display.flipScreenVertically();

  for(int8_t i=0; i<60; i++) {
    setPeriod(i);
  }

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &togglePeriod, true);
  timerAlarmWrite(timer, 1e6, true);
  timerAlarmEnable(timer);

  countdown = 3612;

  /*
  file = new AudioFileSourcePROGMEM( rudolph, strlen_P(rudolph) );
  out = new AudioOutputI2S();
  out->SetPinout(I2S_SCK, I2S_LRCK, I2S_DIN);
  float volume = 0.1;
  out->SetGain((float)volume);
  rtttl = new AudioGeneratorRTTTL();
  rtttl->begin(file, out);
  */

  // Rtttl.play(mario);

}

void loop() {
  if (curPeriod != lastPeriod) {
    sec2human(countdown, countdownString);
    clearTime(oldCountdownString);
    updateTime(countdownString);
    strncpy(oldCountdownString, countdownString, strlen(countdownString));

    if (curPeriod) {
      clearPeriod(59);
    } else {
      setPeriod(59);
    }
    lastPeriod = curPeriod;
  }

  /*
  if (rtttl->isRunning()) {
    if (!rtttl->loop()) {
      rtttl->stop();
      digitalWrite(I2S_EN, 0); // deactivate I2S
    }
  }
  */
  // Rtttl.updateMelody();
}
