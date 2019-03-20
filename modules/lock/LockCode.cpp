#include "LockCode.h"
#include "fonts.h"

LockCode::LockCode(SSD1306Wire *display) {
  draw = Draw();
  setDisplay(display);
}

void LockCode::setDisplay(SSD1306Wire *display) {
  draw.setDisplay(display);
  display = display;
}

void LockCode::drawCodeInput(Point p) {
  int x = p.x;
  int y = p.y;
  int width = SLOT_FRAME_DIM.width;
  int height = SLOT_FRAME_DIM.height;

  for (int i = 0; i < TOTAL_SLOTS; i++) {
    Box slot = slots[i] = {x, y, width, height};
    draw.drawSlotFrame(slot);
    curDigit = String(curCode[i]);
    draw.drawDigit(slot, curDigit);
    x += width + SLOT_FRAME_GAP;
  }
}

void LockCode::updateDigit() {
  digitNeedsUpdate = false;
  Box slot = slots[curSlot];
  draw.clearDigit(slot, curDigit);
  curDigit = String(curCode[curSlot]);
  draw.drawDigit(slot, curDigit);
}

void LockCode::drawCurrentSlotFrame() {
  draw.drawSlotFrame(slots[curSlot]);
}

void LockCode::clearCurrentSlotFrame() {
  draw.clearSlotFrame(slots[curSlot]);
}

void LockCode::selectSlot(unsigned int index) {
  drawCurrentSlotFrame();
  curSlot = index;
  curDigit = String(curCode[curSlot]);
}

unsigned int LockCode::nextSlot() {
  unsigned int index = (curSlot + 1) % TOTAL_SLOTS;
  selectSlot(index);

  return index;
}

bool LockCode::isLastSlot() {
  return (curSlot + 1) == TOTAL_SLOTS;
}

void LockCode::setDigit(unsigned int digit) {
  curCode[curSlot] = (char)(digit + 48);
  updateDigit();
}

void LockCode::countUp() {
  unsigned int nr = (int)curCode[curSlot] - 48;
  nr = (nr + 1) % 10;
  setDigit(nr);
}

void LockCode::countDown() {
  int nr = (int)curCode[curSlot] - 48;
  if (--nr < 0)
    nr = 9;
  setDigit(nr);
}

void LockCode::setCode(String code) {
  this->code = code;
}

bool LockCode::checkCode() {
  return (curCode == code);
}

void LockCode::setup() {
  drawCodeInput(SLOT_FRAME_OFFSET);
}

void LockCode::loop() {
}
