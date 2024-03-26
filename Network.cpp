#include "Network.h"
#include "defines.h"

Network::Network()
{
}

bool Network::connect(const char *_ssid, const char *_pass, uint16_t _timeoutSeconds)
{
    // Connect to Wi-Fi
    DEBUG_PRINT("Connecting to WiFi...");
    WiFi.begin(_ssid, _pass);

    // Initialize variable which will count the timeout
    uint16_t timeoutCounter = 0;

    // Keep checking until we're connected
    while (WiFi.status() != WL_CONNECTED)
    {
        // Wait and inform the user that we're still connecting
        delay(1000);

        // If the timeout was reached, we can't connect!
        // Return false
        timeoutCounter++;
        if (timeoutCounter >= _timeoutSeconds)
        {
            return false;
        }
    }

    // Do one final check to see if we're connected, just in case
    if (WiFi.status() == WL_CONNECTED)
    {
        // Great, we're connected!
        return true;
    }
    else
    {
        // Not connected
        return false;
    }
}

bool Network::isConnected()
{
    // Simply return true if we're connected, false if not
    if (WiFi.status() == WL_CONNECTED)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Network::getTimeAndSaveToRTC(const char *_ntpServer, const char *_timezone, uint16_t _timeout)
{
    // Get time from NTP
    configTime(0, 0, _ntpServer);

    // Set the passed time zone
    // Check zones.csv for which timezones you can pass to this function
    setenv("TZ", _timezone, 1);
    tzset();

    // Now wait until time is set in the RTC
    time_t now = time(nullptr);
    uint32_t rtcCounter = 0;
    while (now < 24 * 3600)
    {
        delay(200); // Wait 200ms
        now = time(nullptr);

        // Count it for the timeout
        // It's multiplied by 5 here because we're using a 200 ms delay to speed things up
        rtcCounter++;
        if(rtcCounter/5 >= _timeout)
        {
            return false;
        }
    }

    // RTC configured correctly!
    return true;
}