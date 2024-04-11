/**
 **************************************************
 *
 * @file        Soldered-Smart-Watch.ino
 * @brief       The main sketch file for the Soldered Smart Watch project. This sketch turns a Dasduino CONNECTPLUS with
 *an OLED display and a gyroscope into a smart watch. Check out the whole project with details on how to build it at
 *xyz.
 *
 * @note        Please check the README for library requirements to compile this sketch.
 *
 *              To build the project you will need:
 *              - Dasduino CONNECTPLUS: https://solde.red/333169
 *              - LSM6DS3 6-DOF breakout: https://solde.red/333115
 *              - OLED I2C 0.96" display: https://solde.red/333099
 *
 * @authors     Robert for soldered.com
 ***************************************************/

// This file is pretty important, it's for global defines and configurations
// If you want the watch to connect to your WiFi, you'll have to edit this file!
#include "src/defines.h"

// Include other external files and libraries
#include "LSM6DS3-SOLDERED.h" // Gyroscope library
#include "src/Display.h"      // Display driver
#include "src/Network.h"      // Network functions
#include "src/WSLED.h"        // Onboard RGB LED driver
#include "time.h"             // For storing time data
#include <RBD_Button.h>       // Button driver
#include <RBD_Timer.h>        // Required for button driver

// Let's declare objects which run the different features of the device
Network network;                // Network functions
Display display;                // OLED display
Soldered_LSM6DS3 gyro;          // Gyroscope
Wsled led;                      // RGB LED
RBD::Button button(BUTTON_PIN); // Button

// Local variable to remember the time when the RTC was last synchronized
time_t lastSyncAttemptTime;

// To check if the button was pressed
volatile bool buttonPressed = false;

// Let's remember the day of the week, so when it changes, we know to reset the step count
uint8_t lastRememberedWeekday = 0;

// Remember if low battery alert is active or not
bool lowBattery = false;

// Setup code, runs only once at startup
void setup()
{
    // Enable Serial communication if DEBUG is enabled
    if (DEBUG)
        Serial.begin(115200);

    // Print hello message to debug serial
    DEBUG_PRINT("Welcome to Soldered Smart Watch!");

    // Let's try to initialize the OLED display
    DEBUG_PRINT("Initializing OLED display...");
    if (!display.begin())
    {
        errorHandling("Couldn't initialize OLED display!");
    }
    DEBUG_PRINT("OLED display initialized!");

    // Let's try to initialize the OLED display
    DEBUG_PRINT("Initializing WSLED...");
    led.begin();
    DEBUG_PRINT("WSLED initialized!");

    // Let's initialize the gyroscope
    DEBUG_PRINT("Initializing gyroscope...");
    display.showLoadingMessage(OLED_GYRO_INIT_MSG); // Show a message on the OLED also
    if (gyro.beginCore() != 0)
    {
        // Couldn't init gyro!
        errorHandling(OLED_GYRO_INIT_ERROR_MSG);
    }
    // Now that the gyro is init'ed, also configure it!
    configGyro();

    // Let's attempt to connect to WiFi
    DEBUG_PRINT("Connecting to WiFi...");
    display.showLoadingMessage(OLED_WIFI_CONNECTING_MSG); // Show a message on the OLED also
    if (!network.connect(ssid, password, WIFI_CONNECT_TIMEOUT_SEC))
    {
        // Couldn't connect!
        errorHandling(OLED_WIFI_CONNECTING_ERROR_MSG);
    }
    DEBUG_PRINT("Connected to WiFi!");

    // Adding this delay helps to make sure we're fully connected to WiFi
    delay(1800);

    // Also, get the time and save it to RTC
    DEBUG_PRINT("Getting time...");
    display.showLoadingMessage(OLED_GETTING_TIME_MSG); // Show a message on the OLED also
    if (!network.getTimeAndSaveToRTC(ntpServer, timeZone, RTC_CONFIG_TIMEOUT_SEC))
    {
        // Couldn't get time from NTP server
        errorHandling(OLED_GETTING_TIME_ERROR_MSG);
    }
    DEBUG_PRINT("Got time and saved to RTC!");

    // Save the last sync attempt time
    lastSyncAttemptTime = time(nullptr);
    pinMode(BATTERY_VOLTAGE_PIN, INPUT);
}

// The main loop of the program
void loop()
{
    // Let's turn off the LED in case it was left on
    led.ledOff();

    // Let's check if low battery alert needs to be on:
    int batteryChargeStateValue = analogRead(BATTERY_VOLTAGE_PIN); // Read the analog value from pin 33
    if (batteryChargeStateValue <= LOW_BATTERY_ANALOG_READ)
    {
        lowBattery = true;
    }
    else
    {
        lowBattery = false;
    }

    // First, let's check if we need to re-sync the RTC via WiFi
    // Subtract the offset in seconds
    time_t currentTime = time(nullptr) - RTC_SECONDS_OFFSET;
    long timeDifference = difftime(currentTime, lastSyncAttemptTime);

    // Let's check if we need to reset the step count
    struct tm *timeinfo = localtime(&currentTime);
    int currentDayOfWeek = timeinfo->tm_wday;
    if (currentDayOfWeek != lastRememberedWeekday)
    {
        // First, save the day of the week
        lastRememberedWeekday = currentDayOfWeek;

        // Now reset the step count
        configGyro(); // Configuring the gyro resets the step count
    }

    // Let's get the currently measured number of steps
    uint32_t numSteps = getNumSteps();

    // Draw the current time and step count, and the low battery alert if so
    display.drawTimeAndStepCount(currentTime, numSteps, lowBattery);

    // Check if it's time to re-sync the RTC
    if (timeDifference >= RTC_SYNC_INTERVAL_SEC)
    {
        DEBUG_PRINT("Time to re-sync the RTC!");

        lastSyncAttemptTime = currentTime; // Remember the time a sync was attempted

        // Show the indicator on the display
        display.drawUpdatingRtcIndicator();

        // Set the flag to remember if we're connected to WiFi
        bool wifiConnected = false; // False by default

        // If we're not connected to Wi-Fi, re-connect
        if (!network.isConnected())
        {
            // If we can connect to WiFi, re-sync the RTC
            if (!network.connect(ssid, password, WIFI_CONNECT_TIMEOUT_SEC))
            {
                // Couldn't connect!
                DEBUG_PRINT("Couldn't connect to WiFi");
                // That's fine, we will keep RTC data
            }
            else
            {
                // Great, we're connected
                DEBUG_PRINT("Connected to WiFi!");
                wifiConnected = true; // Update the flag
            }
        }
        else
        {
            // We're already connected!
            wifiConnected = true;
        }

        if (wifiConnected)
        {
            // Now get the time and save it to RTC
            network.getTimeAndSaveToRTC(ntpServer, timeZone, RTC_CONFIG_TIMEOUT_SEC);
            DEBUG_PRINT("Updated time and saved to RTC!");
        }
    }

    // Now let's wait and periodically check for the button
    for (int i = 0; i < 500; i++)
    {
        // Wait 3 ms 500 times -> 1500 ms total
        delay(3);
        // If the flag was set in the meantime, do button action
        if (button.onPressed())
        {
            // Launch menu which selects feature
            DEBUG_PRINT("Button pressed - going to menu!");
            menu();
        }
    }
}

/**
 * @brief Print the error message on Serial and the OLED and then go to infinite loop
 *
 * @param error The string which describes the error
 */
void errorHandling(const char *error)
{
    DEBUG_PRINT(error);
    display.drawErrorMessage(error);
    // Go to infinite loop
    while (true)
    {
        // Restart the ESP if the button is pressed
        if (button.onPressed())
        {
            esp_restart();
        }
    }
}

/**
 * @brief Configure the gyroscope so it works as a pedometer
 * 
 * @note This function also resets the number of steps counted
 *
 */
void configGyro()
{
    uint8_t errorAccumulator = 0; // Error accumulation variable
    uint8_t dataToWrite = 0;      // Temporary variable

    // Configure range to lower range
    dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_2g;
    // Configure data rate
    dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_26Hz;
    // Now, write the patched together data
    errorAccumulator += gyro.writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dataToWrite);

    // Set the ODR bit
    errorAccumulator += gyro.readRegister(&dataToWrite, LSM6DS3_ACC_GYRO_CTRL4_C);
    dataToWrite &= ~((uint8_t)LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED);

    // Enable embedded functions -- ALSO clears the step count
    errorAccumulator += gyro.writeRegister(LSM6DS3_ACC_GYRO_CTRL10_C, 0x3E);
    // Enable pedometer algorithm
    errorAccumulator += gyro.writeRegister(LSM6DS3_ACC_GYRO_TAP_CFG1, 0x40);

    // If there was an error, go to error handling
    if (errorAccumulator)
    {
        errorHandling("Couldn't configure gyro!");
    }
}

/**
 * @brief Get the number of steps as measured by the gyroscope
 *
 * @return uint16_t
 */
uint16_t getNumSteps()
{
    // Create placeholder variables
    uint8_t readDataByte = 0;
    uint16_t stepsTaken = 0;

    // Read the 16bit value by two 8bit operations
    gyro.readRegister(&readDataByte, LSM6DS3_ACC_GYRO_STEP_COUNTER_H);
    stepsTaken = ((uint16_t)readDataByte) << 8;
    gyro.readRegister(&readDataByte, LSM6DS3_ACC_GYRO_STEP_COUNTER_L);
    stepsTaken |= readDataByte;

    // Return it!
    return stepsTaken;
}

/**
 * @brief The menu function, this is launched when the button is pressed
 * 
 */
void menu()
{
    // Start measuring time
    uint32_t timeout = millis();
    int menuPage = 0; // Start with 0 for first press
    
    // Show the menu page and color
    display.drawMenuPage(menuPage);
    led.showMenuColor(menuPage);

    // Go to infinite loop
    while (true)
    {
        // If the button is pressed, keep counting the menu pages
        if (button.onPressed())
        {
            menuPage++;

            // Watch if the pages roll over.
            if (menuPage >= 4)
            {
                menuPage = 0;
            }

            // Show the current page
            display.drawMenuPage(menuPage);
            led.showMenuColor(menuPage);
            
            // Reset the timer
            timeout = millis();
        }

        // When the timeout is over, launch  the according function
        if (millis() - timeout > MENU_TIMEOUT_MS)
        {
            if (menuPage == 0)
            {
                display.wifiScanner(&button);
                return;
            }
            else if (menuPage == 1)
            {
                display.gyroAnimation(&gyro, &button);
                return;
            }
            else if (menuPage == 2)
            {
                // "Self destruct"
                customFunction();
                return;
            }
            else
            {
                // Go back
                return;
            }
        }
    }
}

/**
 * @brief Write your own implementation here!
 * 
 */
void customFunction()
{
    // Count 3... 2... 1...
    for (int i = 3; i >= 0; i--)
    {
        display.selfDestructMessage(i);

        // The blinks are 200ms, so 5 blinks for 1 second
        led.redBlink();
        led.redBlink();
        led.redBlink();
        led.redBlink();
        led.redBlink();
    }

    // Show that self destruct is over
    display.selfDestructEnd();
    delay(5000); // Wait so that the user sees the message
}