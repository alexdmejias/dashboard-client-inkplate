#pragma once

#include "Arduino.h"
#include "Inkplate.h"

#define TOUCHPAD_WAKE_MASK (int64_t(1) << GPIO_NUM_34)

struct Config
{
    char server[64];
    char password[64];
    char ssid[64];
    int wifiTimeout;
    int sleepTime;
    bool debug;
    float timezoneOffset; // Timezone offset in hours from UTC (e.g., -5 for EST, +5.5 for IST)
    bool showSleepStatus; // Display sleep information before entering deep sleep
};

void log(String msg);
void stopProgram(Inkplate &d);
void handleSleep(Inkplate &d, int time, Config &config);