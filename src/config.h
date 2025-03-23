#pragma once
#include <ArduinoJson.h>
#include "DisplayWrapper.h"
#include "global.h"
#include "draw.h"

void readConfig(DisplayWrapper &d, const char *filename, Config &config);
void saveConfiguration(const char *filename, Config &config);
void readSerialCommands(Config &config);
void printFile(const char *filename);