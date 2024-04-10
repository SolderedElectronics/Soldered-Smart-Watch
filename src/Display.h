#ifndef __SMART_WATCH_DISPLAY__
#define __SMART_WATCH_DISPLAY__

#include "OLED-Display-SOLDERED.h"
#include "LSM6DS3-SOLDERED.h"
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
    void drawErrorMessage(const char *_error);
    void drawMenuPage(uint8_t _menuPageIndex);
    void selfDestructMessage(int _secRemaining);
    void selfDestructEnd();
    void gyroAnimation(Soldered_LSM6DS3 * _gyro);

  private:
    OLED_Display *oledDisplay;
    void project(float *v, float angleX, float angleY, float angleZ, int *x, int *y);
};

#endif