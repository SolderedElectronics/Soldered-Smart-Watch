#include "Display.h"
#include "defines.h"
#include "images.h"

bool Display::begin()
{
    // Let's create the OLED display object
    // Check if oledDisplay already points to an OLED_Display object
    if (oledDisplay != nullptr)
    {
        // Optionally, handle the case where begin is called more than once
        // For example, delete the existing object and create a new one
        // Or simply return true/false depending on how you want to handle this case
        return false; // For now, just indicate failure or that it's already initialized
    }

    // Create a new OLED_Display object and assign its address to oledDisplay
    oledDisplay = new OLED_Display();

    // Perform any initialization required for the OLED_Display object
    bool initSuccess = oledDisplay->begin();

    return initSuccess;
}

void Display::showLoadingMessage(const char *_message)
{
    oledDisplay->clearDisplay(); // Clear the display buffer

    // Draw the new Soldered logo
    oledDisplay->drawBitmap(0, 0, solderedLogo, OLED_WIDTH, OLED_HEIGHT, SSD1306_BLACK, SSD1306_WHITE);

    // Print the message below the logo
    oledDisplay->setCursor(0, 40);
    oledDisplay->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    oledDisplay->print(_message);
    oledDisplay->display(); // Show the Soldered Logo
}

void Display::drawTimeAndStepCount(time_t _currentTime, uint16_t _stepCount)
{
    // Transform time to local time, so we can get the hours and minutes
    struct tm *timeinfo = localtime(&_currentTime);
    // Sprintf it to a formatted string so that time always shows as two numbers
    char timeString[6]; // Buffer to hold the formatted time; HH:MM + null terminator
    sprintf(timeString, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);

    // Let's draw everything on the display
    oledDisplay->clearDisplay(); // Clear the display buffer
    oledDisplay->setCursor(5, 9); // Set the cursor for printing time
    oledDisplay->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    oledDisplay->setTextSize(4); // Set font size to small
    oledDisplay->print(timeString); // Print time
    oledDisplay->setCursor(15,54);
    oledDisplay->setTextSize(1); // Set font size to small
    oledDisplay->print("Steps today: ");
    oledDisplay->print(_stepCount);
    oledDisplay->display();
}

void Display::drawUpdatingRtcIndicator()
{
    // Don't clear the display
    oledDisplay->drawRect(0, 0, 128, 64, SSD1306_WHITE);
    oledDisplay->drawRect(2, 2, 128, 64, SSD1306_WHITE);
    oledDisplay->display();
}