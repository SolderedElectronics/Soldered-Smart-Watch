#include "Display.h"
#include "LSM6DS3-SOLDERED.h" // Gyroscope library
#include "SolderedLogo.h"
#include "defines.h"

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
 */
void Display::drawTimeAndStepCount(time_t _currentTime, uint16_t _stepCount)
{
    // Transform time to local time, so we can get the hours and minutes
    struct tm *timeinfo = localtime(&_currentTime);
    // Sprintf it to a formatted string so that time always shows as two numbers
    char timeString[6]; // Buffer to hold the formatted time; HH:MM + null terminator
    sprintf(timeString, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);

    // Let's draw everything on the display
    oledDisplay->clearDisplay();                             // Clear the display buffer
    oledDisplay->setCursor(5, 9);                            // Set the cursor for printing time
    oledDisplay->setTextColor(SSD1306_WHITE, SSD1306_BLACK); // Set the text color
    oledDisplay->setTextSize(4);                             // Set font size to small
    oledDisplay->print(timeString);                          // Print the time string
    oledDisplay->setCursor(15, 54);                          // Set the cursor to the position for steps
    oledDisplay->setTextSize(1);                             // Set font size to small
    // Print the steps
    oledDisplay->print("Steps today: ");
    oledDisplay->print(_stepCount);

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


    oledDisplay->print("Restart via button...");
    oledDisplay->setCursor(0, 50);
    oledDisplay->print(_error);

    // Show everything on the display
    oledDisplay->display();
}

void Display::drawMenuPage(uint8_t _menuPageIndex)
{
    oledDisplay->clearDisplay();                             // Clear the display buffer
    oledDisplay->setCursor(0, 34);                           // Set the cursor for printing time
    oledDisplay->setTextColor(SSD1306_WHITE, SSD1306_BLACK); // Set the text color accordingly
    oledDisplay->setTextSize(1);                             // Set font size to small
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
    else
    {
        oledDisplay->print(MENU_PAGE_3_TEXT);
    }
    // Show everything on the display
    oledDisplay->display();
}

void Display::selfDestructMessage(int _secRemaining)
{
    oledDisplay->clearDisplay();
    oledDisplay->setCursor(0, 34);
    oledDisplay->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    oledDisplay->setTextSize(1);
    oledDisplay->print("Self destructing in ");
    oledDisplay->print(_secRemaining);
    oledDisplay->display();
}

void Display::selfDestructEnd()
{
    oledDisplay->clearDisplay();
    oledDisplay->setCursor(0, 20);
    oledDisplay->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    oledDisplay->setTextSize(1);
    oledDisplay->print("Just kidding :)");
    oledDisplay->setCursor(0, 40);
    oledDisplay->print("Implement your custom function here!");
    oledDisplay->display();
}

void Display::gyroAnimation(Soldered_LSM6DS3 *_gyro)
{
    float cube[8][3] = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
                        {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1}};
    // Cube edges
    int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Bottom face
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, // Top face
        {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Vertical edges
    };

    float angleX = 0;
    float angleY = 0;
    float angleZ = 0;
    // Also, remember the previous angles
    // This is just to calculate the average between the two in order to smooth out the movement
    float previousAngleX = 0;
    float previousAngleY = 0;
    float previousAngleZ = 0;

    float angleModifier = 0.00008;

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
        // Wait 50ms so the frame rate isn't too fast
        delay(50);
    }
}

// This function projects 3D space onto 2D with a set rotation
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