#ifndef LockCode_h
#define LockCode_h

#include <Arduino.h>
#include <SSD1306Wire.h>

#include "base.h"
#include "Draw.h"

class LockCode {
private:
  SSD1306Wire *display;

  const short SLOT_FRAME_GAP = 5;
  const short TOTAL_SLOTS = 3;

  const Point STATUS_OFFSET = {0, 0};
  const Point SLOT_FRAME_OFFSET = {21, 24};
  const Dimension SLOT_FRAME_DIM = {25, 31};

  unsigned int lastSlot = 0;
  unsigned int curSlot = 0;

  String code = "007";
  String curCode = "000";

  String curDigit = "0";

  Draw draw;
  Box slots[3];

  void drawCodeInput(Point p);
  void updateDigit();

public:
  bool digitNeedsUpdate = false;

  LockCode(SSD1306Wire *display);

  void setDisplay(SSD1306Wire *display);
  void drawCurrentSlotFrame();
  void clearCurrentSlotFrame();
  void selectSlot(unsigned int index);
  unsigned int nextSlot();
  void setDigit(unsigned int digit);
  void countUp();
  void countDown();
  void setCode(String code);
  bool checkCode();
  bool isLastSlot();
  void setup();
  void loop();
};

#endif