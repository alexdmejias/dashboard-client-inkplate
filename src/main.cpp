#include "config.h"
#include "global.h"

#include "Arduino.h"
#include <ArduinoJson.h>
#include "DisplayWrapper.h"
// #include "HTTPClient.h"
// #include "WiFi.h"
// #include "fonts/FreeSans9pt7b.h"
// #include "fonts/FreeSans12pt7b.h"
// #include "fonts/FreeSans24pt7b.h"
// #include "driver/rtc_io.h"
// #include "time.h"

DisplayWrapper display;

const char *filename = "/config.txt"; // SD library uses 8.3 filenames

int sleepFor;
bool inDebugMode = false;
Config config;

unsigned long lastTouchpadCheckTime = 0;
const unsigned long touchpadCheckInterval = 100; // Check every 100 milliseconds

// void connectToWifi(const char *ssid, const char *password, int timeout);
void handleWakeup();
void getImage(const char *server);

void setup()
{
  display.begin(); // Init display library (you should call this function ONLY ONCE)

  // display.sdCardInit();
  Serial.begin(115200);

  handleWakeup();

  // readConfig(display, filename, config);

  // if (inDebugMode)
  // {
  //   display.clearDisplay();
  //   drawErrorMessage(display, "debug mode, touch any pad to exit");
  //   display.display();
  // }
  // else
  // {
  //   connectToWifi(config.ssid, config.password, config.wifiTimeout);
  //   getImage(config.server);

  //   if (config.debug)
  //   {
  //     drawDebugInfo(display, config);
  //   }

  //   display.display();
  //   handleSleep(sleepFor | config.sleepTime | 3000);
  // }
}

void loop()
{
}
//   unsigned long currentMillis = millis();
//   if (currentMillis - lastTouchpadCheckTime >= touchpadCheckInterval)
//   {
//     lastTouchpadCheckTime = currentMillis;

// #if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
//     if (display.readTouchpad(PAD1) || display.readTouchpad(PAD2) || display.readTouchpad(PAD3))
//     {
//       log("exiting debug mode");
//       inDebugMode = false;
//       ESP.restart();
//     }
// #endif
//   }

//   if (Serial.available())
//   {
//     readSerialCommands(config);
//   }
// }
// }

void handleWakeup()
{
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    log("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    log("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    log("Wakeup caused by touchpad");
    inDebugMode = true;
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    log("Wakeup caused by ULP program");
    break;
  default:
    log("Wakeup was not caused by deep sleep");
    break;
  }
}

// void connectToWifi(const char *ssid, const char *password, int timeout)
// {
//   log("Connecting to WiFi...");
//   bool connectedToWifi = WiFi.begin(ssid, password);
//   unsigned long startAttemptTime = millis();

//   while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < (unsigned long)(timeout * 1000))
//   {
//     delay(100);
//   }

//   if (WiFi.status() != WL_CONNECTED)
//   {
//     log("Failed to connect to WiFi");
//     drawErrorMessage(display, String("Error: Failed to connect to WiFi. SSID: ") + ssid);
//     // stopProgram(display);
//   }

//   log("Connected to WiFi");

void getImage(const char *server)
{
  if (!display.drawImage(server, 0, 0, 0, true))
  {
    display.println("Image open error");
  }
  else
  {
    log("Image displayed successfully");
  }
}

// void getImage(const char *server)
// {
//   HTTPClient http;
//   http.getStream().setNoDelay(true);
//   http.getStream().setTimeout(1);
//   const char *headerKeys[] = {"x-sleep-for"};
//   const size_t numberOfHeaders = 1;

//   http.begin(server);
//   http.collectHeaders(headerKeys, numberOfHeaders);

//   int httpCode = http.GET();
//   if (httpCode == 200)
//   {
//     int32_t len = http.getSize();
//     if (len > 0)
//     {
//       if (!display.drawImage(server, 0, 0, 0, true))
//       {
//         display.println("Image open error");
//       }
//       Serial.printf("%s | %i", http.header("x-sleep-for").c_str(), http.header("x-sleep-for").toInt());
//     }
//     else
//     {
//       log("Invalid response length");
//     }
//   }
//   else
//   {
//     log("HTTP error:");
//     printf("HTTP error: %d\n", httpCode);
//     String payload = http.getString();
//     log("Error response: " + payload);
//     drawErrorMessage(display, "HTTP error: " + String(httpCode));
//   }
// }