#include "sleep.h"
#include "global.h"

// Minimum valid Unix timestamp (approx Sep 2001) - used to check if NTP time is synchronized
const time_t MIN_VALID_TIME = 1000000000;

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
  // Note: toInt() returns 0 for invalid strings, which is caught by the <= 0 check
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
  // Note: toInt() returns 0 for invalid strings, which is valid and caught by range validation below
  result.startHour = startTime.substring(0, startColonPos).toInt();
  result.startMinute = startTime.substring(startColonPos + 1).toInt();
  
  // Parse end time (HH:MM)
  int endColonPos = endTime.indexOf(':');
  if (endColonPos == -1) {
    return false;
  }
  result.endHour = endTime.substring(0, endColonPos).toInt();
  result.endMinute = endTime.substring(endColonPos + 1).toInt();
  
  // Validate time values (catches any invalid toInt() results)
  if (result.startHour < 0 || result.startHour > 23 ||
      result.endHour < 0 || result.endHour > 24 ||
      result.startMinute < 0 || result.startMinute > 59 ||
      result.endMinute < 0 || result.endMinute > 59) {
    return false;
  }
  
  // Special case: if endHour is 24 (representing midnight), endMinute must be 0
  if (result.endHour == 24 && result.endMinute != 0) {
    return false;
  }
  
  return true;
}

// Check if current time is within a schedule entry's time range
// Note: endHour can be 24 (representing 24:00 as midnight), which works correctly
// because the comparison is exclusive (<) at the end boundary.
// Example: 18:00-24:00 includes 23:59 but excludes 00:00 (midnight)
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
    if (now < MIN_VALID_TIME) {
      log("Time not synchronized, cannot use schedule format");
      return 0;
    }
    
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    int currentHour = timeinfo.tm_hour;
    int currentMinute = timeinfo.tm_min;
    
    // Format time with zero-padding (e.g., 09:05)
    char timeStr[8];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", currentHour, currentMinute);
    log("Current time (UTC): " + String(timeStr));
    
    // Parse each schedule entry
    int startPos = 0;
    int headerLen = headerValue.length(); // Cache length to avoid repeated calls
    while (startPos < headerLen) {
      int spacePos = headerValue.indexOf(' ', startPos);
      String entry;
      
      if (spacePos == -1) {
        entry = headerValue.substring(startPos);
        startPos = headerLen;
      } else {
        entry = headerValue.substring(startPos, spacePos);
        startPos = spacePos + 1;
      }
      
      ScheduleEntry schedEntry;
      if (parseScheduleEntry(entry, schedEntry)) {
        // Format schedule entry with zero-padded times (e.g., 06:00-18:00=15)
        // Buffer sized to handle large interval values safely
        char scheduleStr[64];
        snprintf(scheduleStr, sizeof(scheduleStr), "%02d:%02d-%02d:%02d=%d minutes",
                 schedEntry.startHour, schedEntry.startMinute,
                 schedEntry.endHour, schedEntry.endMinute,
                 schedEntry.intervalMinutes);
        log("Parsed schedule entry: " + String(scheduleStr));
        
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
