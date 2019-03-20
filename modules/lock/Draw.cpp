#include "Draw.h"
#include "fonts.h"

Draw::Draw() {}
Draw::Draw(SSD1306Wire *display) { setDisplay(display); }

void Draw::setDisplay(SSD1306Wire *display) { this->display = display; }

void Draw::drawSlotFrame(Box slot) {
  display->drawRect(slot.x, slot.y, slot.width, slot.height);
  display->display();
}

void Draw::clearSlotFrame(Box slot) {
  display->setColor(BLACK);
  drawSlotFrame(slot);
  display->setColor(WHITE);
}

void Draw::drawDigit(Box slot, String digit) {
  display->setFont(Unibody8Pro_Regular_24);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(slot.x + 2, slot.y - 4, digit);
  display->display();
}

void Draw::clearDigit(Box slot, String digit) {
  display->setColor(BLACK);
  drawDigit(slot, digit);
  display->setColor(WHITE);
}
