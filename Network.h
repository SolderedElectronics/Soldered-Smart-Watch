#ifndef __SMART_WATCH_NETWORK__
#define __SMART_WATCH_NETWORK__

#include "WiFi.h"

class Network
{
  public:
    Network();
    bool connect(const char *_ssid, const char *_pass, uint16_t _timeoutSeconds);
    bool isConnected();
    bool getTimeAndSaveToRTC(const char *_ntpServer, const char * _timezone, uint16_t _timeout);
};

#endif