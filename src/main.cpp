#ifndef ARDUINO_INKPLATE10
#error "Wrong board selection for this example, please select Inkplate 10 in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library to the sketch
#include "SdFat.h"    //Include library for SD card
#include "driver/rtc_io.h"
#include "Arduino.h"

#include "globals.h"
#include "draw.h"
// #include "network.h"

#include <Update.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>

Inkplate display(INKPLATE_3BIT);
SdFile file;
Draw d;
WiFiManager wm;

char image_server[34] = "http://192.168.0.97:3000";
WiFiManagerParameter custom_image_server("server", "image server", image_server, 34);

// void configModeCallback(WiFiManager *myWiFiManager)
// {
//   Serial.println("Entered config mode");
//   Serial.println(WiFi.softAPIP());

//   Serial.println(myWiFiManager->getConfigPortalSSID());
//   // display.setTextSize(3);
//   // display.setTextColor(0, 7);
//   // display.setCursor(100, 360);
//   // display.println("in config mode");
// }

// void saveConfigCallback()
// {
//   Serial.println("Should save config");
//   Serial.println("Get Params:");
//   Serial.print(custom_mqtt_server.getID());
//   Serial.print(" : ");
//   Serial.println(custom_mqtt_server.getValue());
// }

void setup()
{
  Serial.begin(115200);
  display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
  display.clearDisplay(); // Clear frame buffer of display
  Serial.setDebugOutput(true);
  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  wm.resetSettings();

  wm.addParameter(&custom_image_server);

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  // wm.setAPCallback(configModeCallback);
  // wm.setSaveConfigCallback(saveConfigCallback);
  res = wm.autoConnect("AutoConnectAP"); // password protected ap

  if (!res)
  {
    Serial.println("Failed to connect");
    // ESP.restart();
  }
  else
  {
    // if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");

    d.update(display, custom_image_server.getValue());
  }

  // if (!display.rtcGetSecond())
  // {
  //   connectWifi();

  //   configTime(globals::gmtOffset_sec, globals::daylightOffset_sec, globals::ntpServer);

  //   struct tm timeinfo;
  //   if (!getLocalTime(&timeinfo))
  //   {
  //     Serial.println("Failed to obtain time");
  //     // return;
  //   }

  //   WiFi.disconnect();
  //   // display.rtcSetTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);                               // Send time to RTC
  //   display.rtcSetDate(timeinfo.tm_wday, timeinfo.tm_mday, timeinfo.tm_mon, timeinfo.tm_year + 1900); // Send date to RTC
  // }

  // display.rtcGetRtcData();

  // d.update(display);

  // display.joinAP(globals::ssid, globals::password);

  // d.update(display, custom_mqtt_server.getValue());

  // DO NOT DELETE
  // esp_sleep_enable_timer_wakeup(globals::TIME_TO_SLEEP * globals::uS_TO_S_FACTOR); // Activate wake-up timer
  // esp_deep_sleep_start();                                                          // Put ESP32 into deep sleep. Program stops here.

  Serial.println("End setup");
}

void loop()
{
}
