#pragma once

#include "Arduino.h"
#include "Inkplate.h"

#define TOUCHPAD_WAKE_MASK (int64_t(1) << GPIO_NUM_34)

struct Config
{
    char server[64];
    char password[64];
    char ssid[64];
    int wifiTimeout; // Timeout for WiFi connection (seconds)
    int sleepTime;   // Sleep time (seconds)
    int httpTimeout; // Timeout for HTTP requests (seconds)
    bool showDebug;
    char timezone[64];
    int wakeButtonPin;
    int debugWindow;
    bool otaEnabled;           // Enable OTA updates
    int otaPort;               // OTA port (default: 3232)
    char otaPassword[64];      // OTA password (optional, for security)
    int otaTimeout;            // Seconds to wait for OTA before sleep (default: 30)
};

void log(String msg);
void stopProgram(Inkplate &d);
void handleSleep(int time, int wakeButtonPin);