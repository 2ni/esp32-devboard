#ifndef Draw_h
#define Draw_h

#include <Arduino.h>
#include <SSD1306Wire.h>

#include "base.h"

class Draw {
private:
  SSD1306Wire *display;
public:
  Draw(SSD1306Wire *display);
  void setDisplay(SSD1306Wire *display);
  void drawSlotFrame(Box slot);
  void clearSlotFrame(Box slot);
  void drawDigit(Box slot, String digit);
  void clearDigit(Box slot, String digit);
};

#endif