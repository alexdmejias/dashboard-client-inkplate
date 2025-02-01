// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "Inkplate.h"
#include "SdFat.h"
#include "Arduino.h"
#include <ArduinoJson.h>
#include "HTTPClient.h" //Include library for HTTPClient
#include "WiFi.h"       //Include library for WiFi
#include "fonts/FreeSans9pt7b.h"
#include "fonts/FreeSans12pt7b.h"
#include "fonts/FreeSans24pt7b.h"
#include "driver/rtc_io.h"
#include "time.h"
#include "global.h"
#include "draw.h"

Inkplate display(INKPLATE_3BIT);
SdFile file;

const char *filename = "/config.txt"; // SD library uses 8.3 filenames
Config config;                        // global configuration object

Config defaultConfig = {
    "example.com",           // server
    "fake_password",         // password
    "my_home_network-5g",    // ssid
    30,                      // wifiTimeout
    20,                      // sleepTime
    true,                    // debug
    "EST5EDT,M3.2.0,M11.1.0" // timezone
};

void readConfig(Inkplate &d, const char *filename, Config &config);
void connectToWifi(Inkplate &d, const char *ssid, const char *password, int timeout);
void stopProgram(Inkplate &d);
// void setTime(Inkplate &d);
// void setTimezone(char *timezone);
void getImage(Inkplate &d, const char *server);

void setup()
{
  display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)

  Serial.begin(115200); // Init serial communication
  log("Inkplate SD card example");

  readConfig(display, filename, config);

  connectToWifi(display, config.ssid, config.password, config.wifiTimeout);

  // setTime(display);
  // setTimezone(config.timezone);
  // // drawImage(display, config.server);
  getImage(display, config.server);

  if (config.debug)
  {
    drawDebugInfo(display, config);
  }

  display.display();

  esp_sleep_enable_timer_wakeup(config.sleepTime * 1000000); // Activate wake-up timer -- wake up after 20s here
  esp_deep_sleep_start();                                    // Put ESP32 into deep sleep. Program stops here.
}

void loop()
{
  // Nothing...
}

void connectToWifi(Inkplate &d, const char *ssid, const char *password, int timeout)
{
  log("Connecting to WiFi...");
  bool connectedToWifi = d.connectWiFi(config.ssid, config.password, config.wifiTimeout, true);
  if (!connectedToWifi)
  {
    log("Failed to connect to WiFi");
    drawErrorMessage(d, String("Error: Failed to connect to WiFi. SSID: ") + config.ssid);
    stopProgram(d);
  }

  log("Connected to WiFi");
}

void getImage(Inkplate &d, const char *server)
{
  HTTPClient http;
  // Set parameters to speed up the download process.
  http.getStream().setNoDelay(true);
  http.getStream().setTimeout(1);
  const char *headerKeys[] = {"x-test"};
  const size_t numberOfHeaders = 1;

  http.begin(server);
  http.collectHeaders(headerKeys, numberOfHeaders);

  // Check response code.
  int httpCode = http.GET();
  if (httpCode == 200)
  {
    // Get the response length and make sure it is not 0.
    int32_t len = http.getSize();
    if (len > 0)
    {
      if (!drawImageFromClient(d, http, len))
      {
        // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
        // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no
        // compression!
        d.println("Image open error");
        d.display();
      }
      Serial.printf("%s", http.header("x-test"));
      d.display();
    }
    else
    {
      log("Invalid response length");
      d.display();
    }
  }
  else
  {
    log("HTTP error");
    d.display();
  }
}

void readConfig(Inkplate &d, const char *filename, Config &config)
{
  // Init SD card. Display if SD card is init propery or not.
  if (d.sdCardInit())
  {
    log("SD Card ok! Reading data...");
    // Allocate a temporary JsonDocument
    JsonDocument doc;

    // Try to load text with max lenght of 200 chars.
    if (!file.open(filename, O_RDONLY))
    { // If it fails to open, send error message to display, otherwise read the file.
      log("File open error");
      drawErrorMessage(d, "Error: Could not open file");
      stopProgram(d);
    }
    else
    {
      char text[501];            // Array where data from SD card is stored (max 500 chars here)
      int len = file.fileSize(); // Read how big is file that we are opening
      if (len > 500)
      {
        len = 500; // If it's more than 200 bytes (200 chars), limit to max 200 bytes
      }
      file.read(text, len); // Read data from file and save it in text array
      text[len] = 0;        // Put null terminating char at the and of data

      DeserializationError error = deserializeJson(doc, text);
      if (error)
      {
        log("Failed to read file, using default configuration" + String(error.c_str()));
      }

      log("Copying values to config object");
      if (!doc["server"].is<const char *>() || !doc["ssid"].is<const char *>() || !doc["password"].is<const char *>())
      {
        log("Missing required config values or incorrect type");
        drawErrorMessage(d, "Error: Missing required config values or incorrect type");
        stopProgram(d);
      }
      strlcpy(config.server, doc["server"], sizeof(config.server));
      strlcpy(config.ssid, doc["ssid"], sizeof(config.ssid));
      strlcpy(config.password, doc["password"], sizeof(config.password));
      // strlcpy(config.timezone, doc["timezone"] | defaultConfig.timezone, sizeof(config.timezone));
      config.sleepTime = doc["sleepTime"] | defaultConfig.sleepTime;
      config.wifiTimeout = doc["wifiTimeout"] | defaultConfig.wifiTimeout;
      config.debug = doc["debug"] | defaultConfig.debug;
    }

    // TODO should dump all of the config data
    log("Config data: ");
    Serial.printf("server: %s\nssid: %s\npassword: %s\n", config.server, config.ssid, config.password);
  }
  else
  {
    log("SD Card error!");
    drawErrorMessage(d, "Error: SD Card error");
    stopProgram(d);
  }
}

void stopProgram(Inkplate &d)
{
  d.display();
  d.sdCardSleep();
  // TODO send into deep sleep
  while (true)
    ;
}

// void setTime(Inkplate &d)
// {
//   log("Setting RTC time");
//   // Structure used to hold time information
//   struct tm timeInfo;
//   d.getNTPDateTime(&timeInfo);
//   time_t nowSec;
//   // Fetch current time in epoch format and store it
//   d.getNTPEpoch(&nowSec);
//   // This loop ensures that the NTP time fetched is valid and beyond a certain threshold
//   log(F("Current time: "));
//   log(asctime(&timeInfo));
//   // while (nowSec < 8 * 3600 * 2)
//   // {
//   //   delay(500);
//   //   yield();
//   //   d.getNTPEpoch(&nowSec);
//   // }
//   log("a");
//   gmtime_r(&nowSec, &timeInfo);
//   log("b");
//   d.rtcSetTime(timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
//   d.rtcGetRtcData();
//   log("c");
//   log("timeInfo.tm_hour");
//   log(String(timeInfo.tm_hour, DEC));
//   log(String(timeInfo.tm_hour));
//   log("timeInfo.tm_hour");
//   // d.rtcSetTime(hour, minutes, seconds);    // Send time to RTC
//   // display.rtcSetDate(weekday, day, month, year); // Send date to RTC
//   log(d.rtcGetHour());
//   log("d");
//   log(String(d.rtcGetHour(), DEC));
//   Serial.println(d.rtcGetHour());
//   log("e");
// }

// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
// void setTimezone(char *timezone)
// {
//   Serial.printf("  Setting Timezone to %s\n", timezone);
//   setenv("TZ", timezone, 1); //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
//   tzset();
// }