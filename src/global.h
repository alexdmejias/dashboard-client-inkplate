#pragma once

#include "Arduino.h"
#include "DisplayWrapper.h"

#define TOUCHPAD_WAKE_MASK (int64_t(1) << GPIO_NUM_34)
#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */

struct Config
{
    char server[64];
    char password[64];
    char ssid[64];
    int wifiTimeout;
    int sleepTime;
    bool debug;
    char timezone[64];
};

void stopProgram(DisplayWrapper &d);
void log(String msg);
void handleSleep(int time);