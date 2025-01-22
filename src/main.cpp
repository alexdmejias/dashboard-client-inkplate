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
#include "driver/rtc_io.h"

Inkplate display(INKPLATE_3BIT);
SdFile file;

struct Config
{
  char hostname[64];
  char password[64];
  char ssid[64];
  int sleepTime;
  bool debug;
};

const char *filename = "/config.txt"; // SD library uses 8.3 filenames
Config config;                        // global configuration object

Config defaultConfig = {
    "example.com",        // hostname
    "fake_password",      // password
    "my_home_network-5g", // ssid
    20,                   // sleepTime
    true                  // debug
};

void readConfig(const char *filename, Config &config);
void getStringCenter(Inkplate &d, String buf, int *a, int *b);
void connectToWifi(const char *ssid, const char *password);
void drawImage(Inkplate &d, const char *hostname);
void drawDebugInfo(Inkplate &d);
void drawErrorMessage(Inkplate &d, String buf);
void log(String msg);

void setup()
{
  display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)

  Serial.begin(115200); // Init serial communication
  log("Inkplate SD card example");

  readConfig(filename, config);

  connectToWifi(config.ssid, config.password);

  drawImage(display, config.hostname);

  if (config.debug)
  {
    drawDebugInfo(display);
  }

  display.display();

  // display.sdCardSleep(); // Put sd card in sleep mode
  esp_sleep_enable_timer_wakeup(config.sleepTime * 1000000); // Activate wake-up timer -- wake up after 20s here
  esp_deep_sleep_start();                                    // Put ESP32 into deep sleep. Program stops here.
}

void loop()
{
  // Nothing...
}

void connectToWifi(const char *ssid, const char *password)
{
  log("Connecting to WiFi...");
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, password);
  // TODO there should be a way to determine if the connection was successful after X amount of time
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  log("WiFi OK! Downloading...");
}

void getStringCenter(Inkplate &d, String buf, int *a, int *b)
{
  int16_t x1, y1;
  uint16_t w, h;

  d.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
  *a = (display.width() - w) / 2;
  *b = (display.height() - h) / 2;
}

void readConfig(const char *filename, Config &config)
{
  // Init SD card. Display if SD card is init propery or not.
  if (display.sdCardInit())
  {
    log("SD Card ok! Reading data...");
    // Allocate a temporary JsonDocument
    JsonDocument doc;

    // Try to load text with max lenght of 200 chars.
    if (!file.open(filename, O_RDONLY))
    { // If it fails to open, send error message to display, otherwise read the file.
      log("File open error");
      display.display();
      display.sdCardSleep();
    }
    else
    {
      char text[501];            // Array where data from SD card is stored (max 200 chars here)
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
        log("Failed to read file, using default configuration");
      }

      log("Copying values to config object");
      // TODO should fail ssid, password, and hostname if they are not present
      strlcpy(config.hostname, doc["hostname"] | defaultConfig.hostname, sizeof(config.hostname));
      strlcpy(config.ssid, doc["ssid"] | defaultConfig.ssid, sizeof(config.hostname));
      strlcpy(config.password, doc["password"] | defaultConfig.password, sizeof(config.hostname));
      config.sleepTime = doc["sleepTime"] | defaultConfig.sleepTime;
      config.debug = doc["debug"] | defaultConfig.debug;
    }

    // TODO should dump all of the config data
    log("Config data: ");
    Serial.printf("\nhostname: %s\nssid: %s\npassword: %s\n", config.hostname, config.ssid, config.password);
  }
  else
  { // If card init was not successful, display error on screen, put sd card in sleep mode, and stop the program (using infinite loop)
    // TODO should probably do something else here
    log("SD Card error!");
    display.sdCardSleep();
    while (true)
      ;
  }
}

void drawImage(Inkplate &d, const char *hostname)
{
  log("Will display image now");

  if (!d.drawPngFromWeb(hostname, 0, 0, 0, true))
  {
    log("Image open error");
    drawErrorMessage(d, "Error: Could not draw image");
  }
  log("Image displayed");
}

void drawErrorMessage(Inkplate &d, String message)
{
  d.setTextSize(1);
  d.setFont(&FreeSans12pt7b);
  d.setTextColor(WHITE, BLACK);
  int centerX;
  int centerY;
  getStringCenter(d, message, &centerX, &centerY);

  d.setCursor(centerX, centerY);

  d.println(message);
}

void drawDebugInfo(Inkplate &d)
{
  // log("Displaying debug info");
  log("Displaying debug info");
  d.setTextSize(1);
  d.setFont(&FreeSans9pt7b);
  d.setTextColor(WHITE, BLACK);
  int centerX;
  int centerY;
  String debugString = "server: " + String(config.hostname) + " | ssid: " + config.ssid + " | sleep: " + String(config.sleepTime);
  getStringCenter(d, debugString, &centerX, &centerY);

  d.setCursor(centerX, 810);

  d.println(debugString);
}

void log(String msg)
{
  Serial.println("::::::::::: " + String(msg));
}