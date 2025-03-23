#pragma once
#include "DisplayWrapper.h"
#include "global.h"
#include <Arduino.h>

void getStringCenter(DisplayWrapper &d, String buf, int *a, int *b);
void drawErrorMessage(DisplayWrapper &d, String buf);
void drawDebugInfo(DisplayWrapper &d, Config &config);
void drawImage(DisplayWrapper &d, const char *server);
bool drawImageFromClient(DisplayWrapper &d, HTTPClient &httpClient, int32_t len);