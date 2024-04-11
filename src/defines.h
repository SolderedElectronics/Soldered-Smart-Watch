#ifndef __SMART_WATCH_DEFINES__
#define __SMART_WATCH_DEFINES__

// Set this to false if you don't want debug messages printed on Serial
// Messages are printed at 115200 baud rate
#define DEBUG true
// Function which prints debug via Serial
#define DEBUG_PRINT(x)                                                                                                 \
    if (DEBUG)                                                                                                         \
    {                                                                                                                  \
        Serial.println(x);                                                                                             \
        Serial.flush();                                                                                                \
    }

// WiFi credentials
// The smart watch will attempt to connect to this WiFi to re-sync the time
const static char *ssid = "Soldered";
const static char *password = "dasduino";

// NTP server to use for time synchronization
const static char *ntpServer = "pool.ntp.org";

// Timezone setting for Zagreb, Croatia
// For a list of possible time zones, check zones.csv
const static char *timeZone = "CET-1CEST,M3.5.0,M10.5.0/3";

// Some timeout settings
#define WIFI_CONNECT_TIMEOUT_SEC 10
#define RTC_CONFIG_TIMEOUT_SEC   10

// How often to attempt to re-sync the RTC via WiFi
#define RTC_SYNC_INTERVAL_SEC 2 * 3600 // Sync every 2 hours by default

// An offset in seconds for the RTC
// You can use this to fine-tune the time saved to the RTC
#define RTC_SECONDS_OFFSET 10

// Timeout for the menu, before returning to the main loop
#define MENU_TIMEOUT_MS 1500

// Display settings
#define OLED_WIDTH  128
#define OLED_HEIGHT 64

// Button pin
#define BUTTON_PIN 4

// Battery voltage read pin
#define BATTERY_VOLTAGE_PIN 33

// The value of the ADC reading at which the low battery alert turns on
#define LOW_BATTERY_ANALOG_READ 2000

// Fixed display messages
// Spaces are in front because this makes them easier to print and align in the middle
#define OLED_WIFI_CONNECTING_MSG       "Connecting to WiFi..."
#define OLED_WIFI_CONNECTING_ERROR_MSG "Can't connect!"
#define OLED_GYRO_INIT_MSG             " Initializing gyro.."
#define OLED_GYRO_INIT_ERROR_MSG       "Can't init gyro!"
#define OLED_GETTING_TIME_MSG          "    Getting time..."
#define OLED_GETTING_TIME_ERROR_MSG    "Can't get time!"
#define MENU_PAGE_0_TEXT "     WiFi Scanner"
#define MENU_PAGE_1_TEXT " Gyroscope Animation"
#define MENU_PAGE_2_TEXT "    Self Destruct"
#define MENU_PAGE_3_TEXT "      Exit Menu"

#endif