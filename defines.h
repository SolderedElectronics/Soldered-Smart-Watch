#ifndef __SMART_WATCH_DEFINES__
#define __SMART_WATCH_DEFINES__

// Set this to false if you don't want debug messages printed on Serial
#define DEBUG true

// Function which prints debug via Serial
#define DEBUG_PRINT(x)                                                                                                 \
    if (DEBUG)                                                                                                         \
    {                                                                                                                  \
        Serial.println(x);                                                                                             \
        Serial.flush();                                                                                                \
    }

// Some timeout settings
#define WIFI_CONNECT_TIMEOUT_SEC 10
#define RTC_CONFIG_TIMEOUT_SEC 10

// How often to attempt to re-sync the RTC via WiFi
#define RTC_SYNC_INTERVAL_SECC 2 * 3600 // Sync every 2 hours by default
#define RTC_SYNC_INTERVAL_SEC 30 // Sync every 2 hours by default

// Display settings
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// Display messages
// Spaces are in front because this makes them easier to print and align in the middle
#define OLED_GYRO_INIT_MSG " Initializing gyro.."
#define OLED_WIFI_CONNECTING_MSG "Connecting to WiFi..."
#define OLED_GETTING_TIME_MSG "    Getting time..."


#endif