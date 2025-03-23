#pragma once
#include <ArduinoJson.h>
// #include "DisplayWrapper.h"
#include "global.h"
#include "SdFat.h"

void readConfig(const char *filename, Config &config);
void saveConfiguration(const char *filename, Config &config);
void readSerialCommands(Config &config);
void printFile(const char *filename);
String readUserInput(const char *prompt, unsigned long timeout);