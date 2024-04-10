#ifndef __SMART_WATCH_WSLED__
#define __SMART_WATCH_WSLED__

#include "WS2812-SOLDERED.h"

class Wsled
{
  public:
    Wsled();
    bool begin();
    void ledOff();
    void showMenuColor(uint8_t _menuPage);
    void redBlink();

  private:
    WS2812 * onBoardLed;
};

#endif