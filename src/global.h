#pragma once

#include "Arduino.h"

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

void log(String msg);