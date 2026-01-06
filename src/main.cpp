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
#include "config.h"
#include "http_errors.h"

Inkplate display(INKPLATE_3BIT);

const char *filename = "/config.txt"; // SD library uses 8.3 filenames

int sleepFor;
bool inDebugMode = false;
Config config;

#if defined(ARDUINO_INKPLATE10)
unsigned long lastTouchpadCheckTime = 0;
const unsigned long touchpadCheckInterval = 100; // Check every 100 milliseconds
#endif

void connectToWifi(Inkplate &d, const char *ssid, const char *password, int timeout);
void handleWakeup(Inkplate &d);
void getImage(Inkplate &d, const char *server);
// void setTime(Inkplate &d);
// void setTimezone(char *timezone);

void setup()
{
  display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)

  Serial.begin(115200);

#if defined(ARDUINO_INKPLATE10)
  display.setIntOutput(1, false, false, HIGH, IO_INT_ADDR);
  display.setIntPin(PAD1, RISING, IO_INT_ADDR);
  display.setIntPin(PAD2, RISING, IO_INT_ADDR);
  display.setIntPin(PAD3, RISING, IO_INT_ADDR);
#endif

  handleWakeup(display);

  readConfig(display, filename, config);

  if (inDebugMode)
  {
    display.clearDisplay();
    drawErrorMessage(display, "debug mode, touch any pad to exit");
    display.display();
  }
  else
  {
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
    int sleepDuration = sleepFor;
    if (sleepDuration == 0)
      sleepDuration = config.sleepTime;
    if (sleepDuration == 0)
      sleepDuration = 3000;

    handleSleep(sleepDuration, config.wakeButtonPin);
  }
}

void loop()
{
#if defined(ARDUINO_INKPLATE10)
  unsigned long currentMillis = millis();
  if (currentMillis - lastTouchpadCheckTime >= touchpadCheckInterval)
  {
    lastTouchpadCheckTime = currentMillis;

    if (display.readTouchpad(PAD1) || display.readTouchpad(PAD2) || display.readTouchpad(PAD3))
    {
      log("exiting debug mode");
      inDebugMode = false;
      ESP.restart();
    }
  }
#endif

  if (Serial.available())
  {
    readSerialCommands(config);
  }
}

void handleWakeup(Inkplate &d)
{
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    log("--------------- v     Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    log("--------------- v     Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    // case ESP_SLEEP_WAKEUP_TOUCHPAD:
    log("--------------- v     Wakeup caused by touchpad");
    inDebugMode = true;
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    log("--------------- v     Wakeup caused by ULP program");
    break;
  default:
    log("--------------- v     Wakeup was not caused by deep sleep, default case");
    break;
  }
}

void connectToWifi(Inkplate &d, const char *ssid, const char *password, int timeout)
{
  log("Connecting to WiFi...");
  bool connectedToWifi = d.connectWiFi(ssid, password, timeout, true);
  if (!connectedToWifi)
  {
    log("Failed to connect to WiFi");
    drawErrorMessage(d, String("Error: Failed to connect to WiFi. SSID: ") + ssid);
    stopProgram(d);
  }

  log("Connected to WiFi");
}

void getImage(Inkplate &d, const char *server)
{
  HTTPClient http;
  // Set parameters to speed up the download process.
  http.getStream().setNoDelay(true);
  const int HTTP_TIMEOUT_SECONDS = 15;  // Timeout for image downloads
  http.getStream().setTimeout(HTTP_TIMEOUT_SECONDS);
  const char *headerKeys[] = {"x-sleep-for"};
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
      }
      Serial.printf("%s | %i", http.header("x-sleep-for").c_str(), http.header("x-sleep-for").toInt());
    }
    else
    {
      log("Invalid response length");
    }
  }
  else
  {
    log("HTTP error:");
    printf("HTTP error: %d\n", httpCode);
    String payload = http.getString();
    log("Error response: " + payload);
    handleHttpError(d, httpCode);
  }
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