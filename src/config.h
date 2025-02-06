#pragma once
#include <ArduinoJson.h>
#include "Inkplate.h"
#include "global.h"
#include "draw.h"

void readConfig(Inkplate &d, const char *filename, Config &config);
void saveConfiguration(const char *filename, Config &config);
void readSerialCommands(Config &config);
void printFile(const char *filename);