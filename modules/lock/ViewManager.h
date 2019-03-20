#ifndef ViewManager_h
#define ViewManager_h

#include <SSD1306Wire.h>

class ViewManager {
private:
  SSD1306Wire *display;

public:
  ViewManager(SSD1306Wire *display);

  void setDisplay(SSD1306Wire *display);
  void setView();
};

#endif
