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

Inkplate display(INKPLATE_3BIT);
SdFile file;

struct Config
{
  char hostname[64];
  char password[64];
  char ssid[64];
};

const char *filename = "/config.txt"; // <- SD library uses 8.3 filenames
Config config;                        // <- global configuration object

void readConfig(const char *filename, Config &config);
void drawCentreString(Inkplate &d, String buf);
void connectToWifi(const char *ssid, const char *password);
void draw(Inkplate &d, const char *hostname);

void setup()
{
  display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
  display.clearDisplay(); // Clear frame buffer of display
  display.display();      // Put clear image on display
  display.setTextSize(5);

  Serial.begin(230400); // Init serial communication
  Serial.println(":::::::::::::::::::: Inkplate SD card example");
  readConfig(filename, config);

  connectToWifi(config.ssid, config.password);

  draw(display, config.hostname);

  display.sdCardSleep(); // Put sd card in sleep mode
}

void loop()
{
  // Nothing...
}
void connectToWifi(const char *ssid, const char *password)
{
  Serial.println(":::::::::::::::::::: Connecting to WiFi...");
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(":::::::::::::::::::: WiFi OK! Downloading...");
}

void drawCentreString(Inkplate &d, String buf)
{
  int16_t x1, y1;
  uint16_t w, h;
  d.setTextSize(3);
  d.setTextColor(7, 1);
  d.getTextBounds(buf, 600, 790, &x1, &y1, &w, &h); // calc width of new string
  d.setCursor(600 - w / 2, 790);
  d.println(buf);
}

void readConfig(const char *filename, Config &config)
{
  // Init SD card. Display if SD card is init propery or not.
  if (display.sdCardInit())
  {
    Serial.println(":::::::::::::::::::: SD Card ok! Reading data...");
    // Allocate a temporary JsonDocument
    JsonDocument doc;

    // Try to load text with max lenght of 200 chars.
    if (!file.open("/config.txt", O_RDONLY))
    { // If it fails to open, send error message to display, otherwise read the file.
      Serial.println(":::::::::::::::::::: File open error");
      display.display();
      display.sdCardSleep();
    }
    else
    {
      char text[201];            // Array where data from SD card is stored (max 200 chars here)
      int len = file.fileSize(); // Read how big is file that we are opening
      if (len > 200)
        len = 200;          // If it's more than 200 bytes (200 chars), limit to max 200 bytes
      file.read(text, len); // Read data from file and save it in text array
      text[len] = 0;        // Put null terminating char at the and of data

      DeserializationError error = deserializeJson(doc, text);
      if (error)
      {
        Serial.println(F("Failed to read file, using default configuration"));
      }

      Serial.println(":::::::::::::::::::: did not fail to read config file, copying values to config object");

      // Copy values from the JsonDocument to the Config
      // config.port = doc["port"] | 1234;

      strlcpy(config.hostname, doc["hostname"] | "example.com", sizeof(config.hostname));
      strlcpy(config.ssid, doc["ssid"] | "example.com", sizeof(config.hostname));
      strlcpy(config.password, doc["password"] | "example.com", sizeof(config.hostname));
    }

    Serial.println(":::::::::::::::::::: config data:");
    Serial.println("hostname");
    Serial.println(config.hostname);
    Serial.println("ssid");
    Serial.println(config.ssid);
    Serial.println("password");
    Serial.println(config.password);
  }
  else
  { // If card init was not successful, display error on screen, put sd card in sleep mode, and stop the program (using infinite loop)
    Serial.println(":::::::::::::::::::: SD Card error!");
    display.sdCardSleep();
    while (true)
      ;
  }
}

void draw(Inkplate &d, const char *hostname)
{
  Serial.println(":::::::::::::::::::: will display image promptly");

  if (!display.drawPngFromWeb(config.hostname, 0, 0, 0, true))
  {
    Serial.println(":::::::::::::::::::: Image open error");
    display.display();
  }
  Serial.println(":::::::::::::::::::: Image displayed");
  drawCentreString(display, String(config.hostname));
  display.display();
}