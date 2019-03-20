#include "ViewManager.h" 

ViewManager::ViewManager(SSD1306Wire *display) {
  setDisplay(display);
}

void ViewManager::setDisplay(SSD1306Wire *display) {
  this->display = display;
}

void setView() {}