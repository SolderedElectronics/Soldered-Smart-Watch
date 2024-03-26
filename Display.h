#ifndef __SMART_WATCH_DISPLAY__
#define __SMART_WATCH_DISPLAY__

#include "OLED-Display-SOLDERED.h"
#include "time.h"

class Display
{
  public:
    Display() : oledDisplay(nullptr)
    {
    } // Constructor initializes oledDisplay to nullptr
    ~Display()
    {
        // Destructor to clean up and avoid memory leaks
        delete oledDisplay;
    }
    bool begin();
    void showLoadingMessage(const char * _message);
    void drawTimeAndStepCount(time_t _currentTime, uint16_t _stepCount);
    void drawUpdatingRtcIndicator();

  private:
    OLED_Display *oledDisplay;
};

#endif