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

Inkplate display(INKPLATE_3BIT);

const char *filename = "/config.txt"; // SD library uses 8.3 filenames

int sleepFor;
bool inDebugMode = false;
Config config;

#if defined(ARDUINO_INKPLATE10)
unsigned long lastTouchpadCheckTime = 0;
const unsigned long touchpadCheckInterval = 100; // Check every 100 milliseconds
#endif

// Structure to hold schedule entry data
struct ScheduleEntry {
    int startHour;
    int startMinute;
    int endHour;
    int endMinute;
    int intervalMinutes;
};

void connectToWifi(Inkplate &d, const char *ssid, const char *password, int timeout);
void handleWakeup(Inkplate &d);
void getImage(Inkplate &d, const char *server);
int parseSleepInterval(String headerValue);
bool parseScheduleEntry(String entry, ScheduleEntry &result);
bool isTimeInRange(int currentHour, int currentMinute, const ScheduleEntry &entry);
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

    // Initialize NTP time synchronization for schedule-based intervals
    log("Initializing NTP time synchronization...");
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    
    // Wait for time synchronization with timeout (max 5 seconds)
    int ntpRetries = 0;
    time_t now = time(nullptr);
    while (now < 1000000000 && ntpRetries < 10) {
      delay(500);
      now = time(nullptr);
      ntpRetries++;
    }
    
    if (now >= 1000000000) {
      log("NTP time synchronized successfully");
      struct tm timeinfo;
      gmtime_r(&now, &timeinfo);
      char timeStr[30];
      strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);
      log(String("Current time (UTC): ") + timeStr);
    } else {
      log("NTP time synchronization failed - schedule-based intervals will not work");
    }

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

    handleSleep(sleepDuration);
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
  http.getStream().setTimeout(1);
  const char *headerKeys[] = {"x-sleep-for", "refresh-interval"};
  const size_t numberOfHeaders = 2;

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
      
      // Parse sleep interval from HTTP headers
      String sleepHeader = "";
      if (http.hasHeader("x-sleep-for")) {
        sleepHeader = http.header("x-sleep-for");
        log("Found x-sleep-for header: " + sleepHeader);
      } else if (http.hasHeader("refresh-interval")) {
        sleepHeader = http.header("refresh-interval");
        log("Found refresh-interval header: " + sleepHeader);
      }
      
      if (sleepHeader.length() > 0) {
        int parsedInterval = parseSleepInterval(sleepHeader);
        if (parsedInterval > 0) {
          sleepFor = parsedInterval;
          log("Setting sleep interval from header: " + String(sleepFor) + " seconds");
        } else {
          log("Failed to parse sleep interval from header, will use config value");
        }
      } else {
        log("No sleep interval header found, will use config value");
      }
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
    drawErrorMessage(d, "HTTP error: " + String(httpCode));
  }
}

// Parse individual schedule entry (format: HH:MM-HH:MM=MINUTES)
bool parseScheduleEntry(String entry, ScheduleEntry &result) {
  entry.trim();
  
  // Find the equals sign that separates time range from interval
  int equalsPos = entry.indexOf('=');
  if (equalsPos == -1) {
    return false;
  }
  
  String timeRange = entry.substring(0, equalsPos);
  String intervalStr = entry.substring(equalsPos + 1);
  
  // Parse interval in minutes
  result.intervalMinutes = intervalStr.toInt();
  if (result.intervalMinutes <= 0) {
    return false;
  }
  
  // Find the dash that separates start and end times
  int dashPos = timeRange.indexOf('-');
  if (dashPos == -1) {
    return false;
  }
  
  String startTime = timeRange.substring(0, dashPos);
  String endTime = timeRange.substring(dashPos + 1);
  
  // Parse start time (HH:MM)
  int startColonPos = startTime.indexOf(':');
  if (startColonPos == -1) {
    return false;
  }
  result.startHour = startTime.substring(0, startColonPos).toInt();
  result.startMinute = startTime.substring(startColonPos + 1).toInt();
  
  // Parse end time (HH:MM)
  int endColonPos = endTime.indexOf(':');
  if (endColonPos == -1) {
    return false;
  }
  result.endHour = endTime.substring(0, endColonPos).toInt();
  result.endMinute = endTime.substring(endColonPos + 1).toInt();
  
  // Validate time values
  if (result.startHour < 0 || result.startHour > 23 ||
      result.endHour < 0 || result.endHour > 24 ||
      result.startMinute < 0 || result.startMinute > 59 ||
      result.endMinute < 0 || result.endMinute > 59) {
    return false;
  }
  
  return true;
}

// Check if current time is within a schedule entry's time range
bool isTimeInRange(int currentHour, int currentMinute, const ScheduleEntry &entry) {
  int currentMinutes = currentHour * 60 + currentMinute;
  int startMinutes = entry.startHour * 60 + entry.startMinute;
  int endMinutes = entry.endHour * 60 + entry.endMinute;
  
  return currentMinutes >= startMinutes && currentMinutes < endMinutes;
}

// Parse sleep interval from header value
// Supports simple formats like "20s", "5m", "2h", "1d"
// Also supports schedule format: "00:00-06:00=480 06:00-18:00=15 18:00-24:00=30"
int parseSleepInterval(String headerValue) {
  headerValue.trim();
  
  if (headerValue.length() == 0) {
    return 0;
  }
  
  log("Parsing sleep interval: " + headerValue);
  
  // Check if this is a schedule format (contains space and '=')
  if (headerValue.indexOf(' ') != -1 && headerValue.indexOf('=') != -1) {
    log("Detected schedule format");
    
    // Get current time
    time_t now = time(nullptr);
    if (now < 1000000000) {
      log("Time not synchronized, cannot use schedule format");
      return 0;
    }
    
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    int currentHour = timeinfo.tm_hour;
    int currentMinute = timeinfo.tm_min;
    
    log("Current time (UTC): " + String(currentHour) + ":" + String(currentMinute));
    
    // Parse each schedule entry
    int startPos = 0;
    while (startPos < headerValue.length()) {
      int spacePos = headerValue.indexOf(' ', startPos);
      String entry;
      
      if (spacePos == -1) {
        entry = headerValue.substring(startPos);
        startPos = headerValue.length();
      } else {
        entry = headerValue.substring(startPos, spacePos);
        startPos = spacePos + 1;
      }
      
      ScheduleEntry schedEntry;
      if (parseScheduleEntry(entry, schedEntry)) {
        log("Parsed schedule entry: " + String(schedEntry.startHour) + ":" + 
            String(schedEntry.startMinute) + "-" + String(schedEntry.endHour) + ":" + 
            String(schedEntry.endMinute) + "=" + String(schedEntry.intervalMinutes) + " minutes");
        
        if (isTimeInRange(currentHour, currentMinute, schedEntry)) {
          int intervalSeconds = schedEntry.intervalMinutes * 60;
          log("Matched schedule entry! Using interval: " + String(intervalSeconds) + " seconds");
          return intervalSeconds;
        }
      } else {
        log("Failed to parse schedule entry: " + entry);
      }
    }
    
    log("No matching schedule entry found for current time");
    return 0;
  }
  
  // Simple format parsing (e.g., "20s", "5m", "2h", "1d")
  char lastChar = headerValue.charAt(headerValue.length() - 1);
  String numPart = headerValue.substring(0, headerValue.length() - 1);
  int value = numPart.toInt();
  
  if (value <= 0) {
    log("Invalid numeric value in sleep interval");
    return 0;
  }
  
  int seconds = 0;
  
  switch (lastChar) {
    case 's':
    case 'S':
      seconds = value;
      log("Parsed as seconds: " + String(seconds));
      break;
    case 'm':
    case 'M':
      seconds = value * 60;
      log("Parsed as minutes: " + String(seconds) + " seconds");
      break;
    case 'h':
    case 'H':
      seconds = value * 3600;
      log("Parsed as hours: " + String(seconds) + " seconds");
      break;
    case 'd':
    case 'D':
      seconds = value * 86400;
      log("Parsed as days: " + String(seconds) + " seconds");
      break;
    default:
      log("Unknown time unit: " + String(lastChar));
      return 0;
  }
  
  return seconds;
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