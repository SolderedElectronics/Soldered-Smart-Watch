#include "WSLED.h"

Wsled::Wsled()
{
}

bool Wsled::begin()
{
    onBoardLed = new WS2812(1, LEDWS_BUILTIN);
    onBoardLed->begin();
    onBoardLed->setBrightness(254);
    onBoardLed->clear();
    onBoardLed->show();
}

void Wsled::ledOff()
{
    onBoardLed->clear();
    onBoardLed->show();
}

void Wsled::showMenuColor(uint8_t _menuPage)
{
    if (_menuPage == 0)
    {
        onBoardLed->setPixelColor(0, onBoardLed->Color(0, 255, 255));
    }
    else if (_menuPage == 1)
    {
        onBoardLed->setPixelColor(0, onBoardLed->Color(255, 165, 0));
    }
    else if (_menuPage == 2)
    {
        onBoardLed->setPixelColor(0, onBoardLed->Color(255, 0, 255));
    }
    else
    {
        onBoardLed->setPixelColor(0, onBoardLed->Color(255, 255, 255));
    }
    onBoardLed->show();
}

void Wsled::redBlink()
{
    unsigned long startTime = millis();
    unsigned long endTime = startTime + 200; // 200 ms from start
    unsigned long currentTime = startTime;

    while (currentTime <= endTime)
    {
        // Calculate how far along we are in the fade process (0.0 to 1.0)
        float progress = float(currentTime - startTime) / (endTime - startTime);

        // Calculate the current red value (255 at start, 0 at end)
        int redValue = 255 * (1 - progress);

        // Update the LED color
        onBoardLed->setPixelColor(0, onBoardLed->Color(redValue, 0, 0));
        onBoardLed->show();

        // Wait a bit before updating again
        delay(5); // Small delay to control fading speed

        // Update current time
        currentTime = millis();
    }

    // Ensure the LED is off after the loop completes
    onBoardLed->setPixelColor(0, onBoardLed->Color(0, 0, 0));
    onBoardLed->show();
}