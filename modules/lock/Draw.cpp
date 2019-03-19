#include "Draw.h"
#include "fonts.h"

Draw::Draw(SSD1306Wire *display) {
  this->setDisplay(display);
}

void Draw::setDisplay(SSD1306Wire *display) {
  this->display = display;
}

void Draw::drawSlotFrame(Box slot)
{
  this->display->drawRect(slot.x, slot.y, slot.width, slot.height);
  this->display->display();
}

void Draw::clearSlotFrame(Box slot)
{
  this->display->setColor(BLACK);
  this->drawSlotFrame(slot);
  this->display->setColor(WHITE);
}

void Draw::drawDigit(Box slot, String digit)
{
  this->display->setFont(Unibody8Pro_Regular_24);
  this->display->setTextAlignment(TEXT_ALIGN_LEFT);
  this->display->drawString(slot.x + 2, slot.y - 4, digit);
  this->display->display();
}

void Draw::clearDigit(Box slot, String digit)
{
  this->display->setColor(BLACK);
  this->drawDigit(slot, digit);
  this->display->setColor(WHITE);
}
