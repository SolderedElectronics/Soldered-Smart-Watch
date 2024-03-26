// Include external files
#include "Display.h"
#include "Network.h" // Network functions
#include "defines.h" // Global defines
#include "images.h"

// Include the required libraries
#include "LSM6DS3-SOLDERED.h" // For the LSM6DS3
#include "time.h"             // For storing time data

// Objects which run the different features of the device
Network network;
Display display;
Soldered_LSM6DS3 gyro;

// Replace these with your WiFi network settings
const char *ssid = "Soldered";
const char *password = "dasduino";

// NTP server to use for time synchronization
const char *ntpServer = "pool.ntp.org";

// Timezone setting for Zagreb, Croatia
// For a list of possible time zones, check zones.csv
const char *timeZone = "CET-1CEST,M3.5.0,M10.5.0/3";

// To remember the time when the RTC was last synchronized
time_t lastSyncAttemptTime;

void setup()
{
    // Enable Serial communication if DEBUG is enabled
    if (DEBUG)
        Serial.begin(115200);

    // Print hello message to debug serial
    DEBUG_PRINT("Welcome to Soldered Smart Watch!");

    // Let's initialize the OLED display
    if (!display.begin())
    {
        errorHandling("Couldn't initialize OLED display!");
    }
    DEBUG_PRINT("OLED display initialized!");

    // Let's initialize the gyroscope
    display.showLoadingMessage(OLED_GYRO_INIT_MSG);
    DEBUG_PRINT("Initializing gyroscope...");
    delay(1000); // So that the user can read the message
    if (gyro.beginCore() != 0)
    {
        // Couldn't connect!
        // Display here as well
        errorHandling("Can't init gyro!");
    }
    configGyro(); // Also configure it!

    // Let's attempt to connect to WiFi
    display.showLoadingMessage(OLED_WIFI_CONNECTING_MSG);
    DEBUG_PRINT("Connecting to WiFi...");
    delay(1000); // So that the user can read the message
    if (!network.connect(ssid, password, WIFI_CONNECT_TIMEOUT_SEC))
    {
        // Couldn't connect!
        // Display here as well
        errorHandling("Can't connect to WiFi!");
    }
    DEBUG_PRINT("Connected to WiFi!");

    // Also, get the time and save it to RTC
    display.showLoadingMessage(OLED_GETTING_TIME_MSG);
    if (!network.getTimeAndSaveToRTC(ntpServer, timeZone, RTC_CONFIG_TIMEOUT_SEC))
    {
        // Display the error also, then restart?
        errorHandling("Couldn't save time to RTC!");
    }
    DEBUG_PRINT("Got time and saved to RTC!");
    delay(1500); // Add a short delay here so the user can read the message

    // Save the last sync attempt time
    lastSyncAttemptTime = time(nullptr);
}

/**
 * The main loop runs repteadly and does the following:
 *  -Checks if RTC needs to be re-synced via WiFi
 *  -Update the display with the time and number of steps
 */
void loop()
{
    // First, let's check if we need to re-sync the RTC via WiFi
    time_t currentTime = time(nullptr);
    long timeDifference = difftime(currentTime, lastSyncAttemptTime);

    // Let's get the currently measured number of steps
    uint16_t numSteps = getNumSteps();

    // Draw the current time and step count
    display.drawTimeAndStepCount(currentTime, numSteps);

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
            // If we can connect to WiFi, re-sync the RTc
            if (!network.connect(ssid, password, WIFI_CONNECT_TIMEOUT_SEC))
            {
                // Couldn't connect!
                // Doesn't matter
                // TODO Counter for sync
                DEBUG_PRINT("Couldn't connect to WiFi");
                // Don't change the flag
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

        // Also, get the time and save it to RTC
        if (!network.getTimeAndSaveToRTC(ntpServer, timeZone, RTC_CONFIG_TIMEOUT_SEC))
        {
            // Display the error also, then restart?
            errorHandling("Couldn't update time to RTC!");
        }
        DEBUG_PRINT("Updated time and saved to RTC!");
    }
    delay(1500);
}

void errorHandling(const char *error)
{
    DEBUG_PRINT(error);
    // Oled
    while (true)
        ;
    // esp_restart(); // Restart everything...
}

/**
 * @brief Configure the gyroscope so it works as a pedometer
 *
 */
void configGyro()
{
    uint8_t errorAccumulator = 0; // Error accumulation variable
    uint8_t dataToWrite = 0;      // Temporary variable

    // Configure range to lower range
    dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_2g;
    // Configure data rate to low for lower power consumption
    dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_13Hz;
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