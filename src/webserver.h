#pragma once

#include <ESPAsyncWebServer.h>
#include "global.h"
#include "Inkplate.h"

// Initialize and start the web server
void setupWebServer(AsyncWebServer &server, Config &config);

// Connect to WiFi or create AP
bool connectToWifiForDebug(Inkplate &d, const char *ssid, const char *password, int timeout, String &ipAddress, String &connectionMode);
