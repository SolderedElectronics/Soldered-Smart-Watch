#include "Display.h"
#include "LSM6DS3-SOLDERED.h"
#include "images.h"
#include "defines.h"
#include <WiFi.h>

/**
 * @brief Initialize the OLED display
 *
 * @return true if it was successful
 * @return false if it failed
 */
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

/**
 * @brief Show a message for the loading screen, with the Soldered Logo
 *
 * @param _message The loading message to print, add leading spaces to center it
 */
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

/**
 * @brief The main drawing function which draws time and step count
 *
 * @param _currentTime The time to draw
 * @param _stepCount The number of steps to print
 * @param _lowBattery To draw the low battery indicator or not
 */
void Display::drawTimeAndStepCount(time_t _currentTime, uint32_t _stepCount, bool _lowBattery)
{
    // Transform time to local time, so we can get the hours and minutes
    struct tm *timeinfo = localtime(&_currentTime);
    char timeString[6]; // Buffer to hold the formatted time; HH:MM + null terminator
    char dateString[8]; // Buffer to hold the formatted date; HH:MM + null terminator
    // Print the time and date in the set format
    sprintf(timeString, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
    sprintf(dateString, "%02d.%02d.", timeinfo->tm_mday, timeinfo->tm_mon + 1);

    // Let's draw everything on the display
    oledDisplay->clearDisplay();                             // Clear the display buffer
    oledDisplay->setCursor(5, 9);                            // Set the cursor for printing time
    oledDisplay->setTextColor(SSD1306_WHITE, SSD1306_BLACK); // Set the text color
    oledDisplay->setTextSize(4);                             // Set font size to large
    oledDisplay->print(timeString);                          // Print the time string
    oledDisplay->setTextSize(1);                             // Set font size back to small
    oledDisplay->setCursor(2, 54);                           // Set the cursor to the position for steps
    oledDisplay->print(dateString);                          // Print the date
    oledDisplay->print("  Steps: ");                         // Also print the number of steps
    oledDisplay->print(_stepCount);

    // Also draw two lines to separate the top from the bottom
    oledDisplay->drawLine(0, 45, 130, 45, SSD1306_WHITE);
    oledDisplay->drawLine(0, 47, 130, 47, SSD1306_WHITE);

    // Draw low battery alert if it's required
    if(_lowBattery)
    {
        oledDisplay->drawBitmap(51, 37, epd_bitmap_low_batt_alert, 23, 12, SSD1306_BLACK, SSD1306_WHITE);
    }

    oledDisplay->display(); // Show everything on the display
}

/**
 * @brief This function draws a rectangle around the border while the watch is sync-ing
 *
 */
void Display::drawUpdatingRtcIndicator()
{
    // Draw two rectangles
    oledDisplay->drawRect(0, 0, 128, 64, SSD1306_WHITE);
    oledDisplay->drawRect(2, 2, 128, 64, SSD1306_WHITE);

    oledDisplay->display(); // Show everything on the display
}

/**
 * @brief In case there's an error, this function will print the error message
 *
 * @param _error The description of the erroir
 */
void Display::drawErrorMessage(const char *_error)
{
    oledDisplay->clearDisplay();                             // Clear the display buffer
    oledDisplay->setCursor(0, 0);                            // Set the cursor for printing time
    oledDisplay->setTextColor(SSD1306_WHITE, SSD1306_BLACK); // Set the text color accordingly
    oledDisplay->setTextSize(1);                             // Set font size to small

    // Print the error
    oledDisplay->print("ERROR:");
    oledDisplay->setCursor(0, 20);
    oledDisplay->print(_error);

    // Print instructions to reset via button
    oledDisplay->setCursor(0, 50);
    oledDisplay->print("Restart via button...");

    // Show everything on the display
    oledDisplay->display();
}

/**
 * @brief Print the page of the menu we're currently on
 *
 * @param _menuPageIndex The index number of the menu page
 */
void Display::drawMenuPage(uint8_t _menuPageIndex)
{
    oledDisplay->clearDisplay();                             // Clear the display buffer
    oledDisplay->setCursor(0, 34);                           // Set the cursor for a centered print
    oledDisplay->setTextColor(SSD1306_WHITE, SSD1306_BLACK); // Set the text color accordingly
    oledDisplay->setTextSize(1);                             // Set font size to small

    // Depending on the menu page print the according text
    // Check defines.h if you want to change the text
    if (_menuPageIndex == 0)
    {
        oledDisplay->print(MENU_PAGE_0_TEXT);
    }
    else if (_menuPageIndex == 1)
    {
        oledDisplay->print(MENU_PAGE_1_TEXT);
    }
    else if (_menuPageIndex == 2)
    {
        oledDisplay->print(MENU_PAGE_2_TEXT);
    }
    // Add more pages if you want!
    else
    {
        oledDisplay->print(MENU_PAGE_3_TEXT);
    }

    // Show everything on the display
    oledDisplay->display();
}

/**
 * @brief This function prints the countdown for the self destruct message
 *
 * @param _secRemaining the number of seconds remaining in the countdown
 */
void Display::selfDestructMessage(int _secRemaining)
{
    // Clear the display, position the cursors correctly and print the text
    oledDisplay->clearDisplay();
    oledDisplay->setCursor(0, 34);
    oledDisplay->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    oledDisplay->setTextSize(1);
    oledDisplay->print("Self destructing in ");
    oledDisplay->print(_secRemaining);
    oledDisplay->display(); // Show it on the display
}

/**
 * @brief This function prints the ending of the self destruct message
 *
 */
void Display::selfDestructEnd()
{
    // Clear the display, position the cursors correctly and print the text
    oledDisplay->clearDisplay();
    oledDisplay->setCursor(0, 20);
    oledDisplay->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    oledDisplay->setTextSize(1);
    oledDisplay->print("Just kidding :)");
    oledDisplay->setCursor(0, 40);
    oledDisplay->print("Implement your custom function here!");
    oledDisplay->display(); // Show it on the display
}

/**
 * @brief This function animates a 3D projected cube on the display from gyroscope data
 *
 * @note  Borrowed from Inkplate 4TEMPERA's gyroscope example!
 *
 * @param _gyro Pointer to the gyroscope object
 * @param button Pointer to the button object, so we know when to exit the function
 */
void Display::gyroAnimation(Soldered_LSM6DS3 *_gyro, RBD::Button *button)
{
    // Variables which are used for drawing the 3D Cube

    // Cube vertices
    float cube[8][3] = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
                        {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1}};
    // Cube edges
    int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Bottom face
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, // Top face
        {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Vertical edges
    };

    // Variables for the angles at which the cube gets projected
    float angleX = 0;
    float angleY = 0;
    float angleZ = 0;

    // Also, remember the previous angles
    // This is just to calculate the average between the two in order to smooth out the movement
    float previousAngleX = 0;
    float previousAngleY = 0;
    float previousAngleZ = 0;

    // This value multiplies the accelerometer readings to help project the cube in the orientation of the accelerometer
    // If you want accelerometer movements to have more effect on the cube's retation, increase this
    // And vice versa
    float angleModifier = 0.00008;

    // Go to infinite loop which projects the cube
    while (true)
    {
        // First, clear what was previously in the frame buffer
        oledDisplay->clearDisplay();

        // Read values from the accelerometer
        float accelX = _gyro->readRawAccelX();
        float accelY = _gyro->readRawAccelY();
        float accelZ = _gyro->readRawAccelZ();

        // Let's draw the cube!
        // Compute the angle modifier variables from the accelerometer data
        angleX = accelX * angleModifier;
        angleY = accelY * angleModifier;
        angleZ = accelZ * angleModifier;

        // Calculate the average between the previous
        // This makes the movement smoother
        angleX = (angleX + previousAngleX) / 2;
        angleY = (angleY + previousAngleY) / 2;
        angleZ = (angleZ + previousAngleZ) / 2;

        // Remember the value for the next loop
        previousAngleX = angleX;
        previousAngleY = angleY;
        previousAngleZ = angleZ;

        // Let's project the cube's edges!
        // For each edge...
        for (int i = 0; i < 12; i++)
        {
            // Get the start and end vertices
            float *v1 = cube[edges[i][0]];
            float *v2 = cube[edges[i][1]];

            // Rotate and project the vertices to 2D
            int x1, y1, x2, y2;

            // Project it, notice that X, Y and Z are rearranged here and not in the default order
            // This is due to the orientation of the gyroscope on the actual board
            project(v1, angleY, angleZ, angleX, &x1, &y1);
            project(v2, angleY, angleZ, angleX, &x2, &y2);

            // Draw the edge
            oledDisplay->drawLine(x1, y1, x2, y2, SSD1306_WHITE);
        }

        oledDisplay->display();
        // Wait 30ms so the frame rate isn't too fast
        delay(30);

        // If the button is pressed, exit the function
        if (button->onPressed())
        {
            return;
        }
    }
}

/**
 * @brief This function projects 3D space onto 2D with a set rotation
 *
 */
void Display::project(float *v, float angleX, float angleY, float angleZ, int *x, int *y)
{
    // Rotate the vertex around the X axis
    float xr = v[0];
    float yr = v[1] * cos(angleX) - v[2] * sin(angleX);
    float zr = v[1] * sin(angleX) + v[2] * cos(angleX);

    // Rotate the vertex around the Y axis
    float xrr = xr * cos(angleY) + zr * sin(angleY);
    float yrr = yr;
    float zrr = -xr * sin(angleY) + zr * cos(angleY);

    // Rotate the vertex around the Z axis
    float xrrr = xrr * cos(angleZ) - yrr * sin(angleZ);
    float yrrr = xrr * sin(angleZ) + yrr * cos(angleZ);
    float zrrr = zrr;

    // Project the vertex to 2D
    float z = 4 / (4 + zrrr);
    *x = xrrr * z * 18 + OLED_WIDTH / 2;
    *y = yrrr * z * 18 + OLED_HEIGHT / 2;
}

/**
 * @brief This function scans for wifi networks and prints the results on the display
 *
 * @param button pointer to the button - so we know when to exit the function
 */
void Display::wifiScanner(RBD::Button *button)
{
    // Let's set up the display for printing
    oledDisplay->clearDisplay();
    oledDisplay->setCursor(0, 0);
    oledDisplay->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    oledDisplay->setTextSize(1);

    // Print text so the users knows what's going on
    oledDisplay->print("Scanning...");
    oledDisplay->display(); // Show it on the display

    // Again, prepare the display for printing
    oledDisplay->clearDisplay();
    oledDisplay->setCursor(0, 0);

    // Scan for available WiFi networks
    int numNetworks = WiFi.scanNetworks();
    if (numNetworks == 0)
    {
        // If there are no networks found, just notify the user
        oledDisplay->print("No networks found");
        oledDisplay->display();
    }
    else
    {
        // Networks have been found!
        oledDisplay->print("Networks found:");

        // Let's print them
        oledDisplay->setCursor(0, 10);
        // Disable text wrapping for this
        oledDisplay->setTextWrap(false);

        // Print all networks but not more than 5
        for (int i = 0; i < numNetworks && i < 5; i++)
        {
            // If the WiFi name is empty, skip it!
            if (WiFi.SSID(i).length() == 0)
                continue;

            // Print the WiFi name (SSID)
            oledDisplay->print(WiFi.SSID(i));
            oledDisplay->display();

            // Manually go to new line
            oledDisplay->setCursor(0, 20 + 10 * i);

            // Wait a bit for dramatic effect
            delay(75);
        }
        // Turn text wrap back on
        oledDisplay->setTextWrap(true);
    }

    // Now wait for user input
    while (true)
    {
        // If the button is pressed, exit the function
        if (button->onPressed())
        {
            return;
        }
    }
}
