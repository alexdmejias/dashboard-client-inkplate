#pragma once
#include "Inkplate.h"
#include "global.h"

void getStringCenter(Inkplate &d, String buf, int *a, int *b);
void drawErrorMessage(Inkplate &d, String buf);
void drawSleepStatus(Inkplate &d, int sleepDuration, float timezoneOffset);
void drawDebugInfo(Inkplate &d, Config &config);
void drawImage(Inkplate &d, const char *server);
bool drawImageFromClient(Inkplate &d, HTTPClient &httpClient, int32_t len);
