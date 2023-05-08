#ifndef ARDUINO_INKPLATE10
#error "Wrong board selection for this example, please select Inkplate 10 in the boards menu."
#endif

#define ESP_DRD_USE_SPIFFS true

#include "Inkplate.h" //Include Inkplate library to the sketch
// #include "SdFat.h"    //Include library for SD card
#define FS_NO_GLOBALS
#include <FS.h>
#include <SPIFFS.h>
#include "driver/rtc_io.h"
#include "Arduino.h"

#include "globals.h"
#include "draw.h"
// #include "network.h"

#include <Update.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
// #include <ESP_DoubleResetDetector.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

Inkplate display(INKPLATE_3BIT);
SdFile file;
Draw d;
WiFiManager wm;

// Number of seconds after reset during which a
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 10

// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

// DoubleResetDetector *drd;

char image_server[34] = "http://192.168.0.97:3000";

const long interval = 5000;

// flag for saving data
bool shouldSaveConfig = false;

WiFiManagerParameter custom_image_server("server", "image server", image_server, 34);

void saveConfigCallback()
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void saveConfigFile()
{
  DynamicJsonDocument jsonBuffer(1024);

  jsonBuffer["image_server"] = image_server;

  fs::File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile)
  {
    Serial.println("failed to open config file for writing");
  }

  serializeJson(jsonBuffer, Serial);
  serializeJson(jsonBuffer, configFile);

  configFile.close();
}

// Loads custom parameters from /config.json on SPIFFS
bool loadConfigFile()
{
  Serial.println("mounting FS...");

  if (SPIFFS.begin())
  {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json"))
    {
      // file exists, reading and loading
      Serial.println("reading config file");
      fs::File configFile = SPIFFS.open("/config.json", "r");
      if (configFile)
      {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);

        DynamicJsonDocument json(1024);
        auto deserializeError = deserializeJson(json, buf.get());
        serializeJson(json, Serial);
        if (!deserializeError)
        {
          Serial.println("\nparsed json");
          strcpy(image_server, json["image_server"]);

          return true;
        }
        else
        {
          Serial.println("failed to load json config");
        }
        configFile.close();
      }
    }
  }
  else
  {
    Serial.println("failed to mount FS");
  }

  return false;
}

// This gets called when the config mode is launced, might
// be useful to update a display with this info.
void configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println("Entered Conf Mode");

  Serial.print("Config SSID: ");
  Serial.println(myWiFiManager->getConfigPortalSSID());

  Serial.print("Config IP Address: ");
  Serial.println(WiFi.softAPIP());
}

void setup()
{
  SPIFFS.begin(true);
  Serial.begin(115200);
  display.begin();
  display.clearDisplay();
  Serial.setDebugOutput(true);

  bool forceConfig = false;

  // drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);
  // if (drd->detectDoubleReset())
  // {
  //   Serial.println(F("Forcing config mode as there was a Double reset detected"));
  //   forceConfig = true;
  // }

  bool spiffsSetup = loadConfigFile();
  if (!spiffsSetup)
  {
    Serial.println(F("Forcing config mode as there is no saved config"));
    forceConfig = true;
  }

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

  wm.setSaveConfigCallback(saveConfigCallback);
  // set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wm.setAPCallback(configModeCallback);

  wm.addParameter(&custom_image_server);

  if (forceConfig)
  {
    if (!wm.startConfigPortal("::startConfigPortal"))
    {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      // reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
  }
  else
  {
    if (!wm.autoConnect("::autoConnect"))
    {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      // if we still have not connected restart and try all over again
      ESP.restart();
      delay(5000);
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  strcpy(image_server, custom_image_server.getValue());

  if (shouldSaveConfig)
  {
    saveConfigFile();
  }

  Serial.println("connected...yeey :)");
  d.update(display, custom_image_server.getValue());

  // save the custom parameters to FS
  if (shouldSaveConfig)
  {
    saveConfigFile();
  }

  // DO NOT DELETE
  // esp_sleep_enable_timer_wakeup(globals::TIME_TO_SLEEP * globals::uS_TO_S_FACTOR); // Activate wake-up timer
  // esp_deep_sleep_start();                                                          // Put ESP32 into deep sleep. Program stops here.

  Serial.println("End setup");
}

unsigned long previousMillis = 0;

void loop()
{

  unsigned long currentMillis = millis();

  Serial.println("in loop");

  if (currentMillis - previousMillis >= interval)
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    d.update(display, custom_image_server.getValue());
  }
}
