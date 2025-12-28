#pragma once
#include "Arduino.h"

// Minimum valid Unix timestamp (approx Sep 2001) - used to check if NTP time is synchronized
extern const time_t MIN_VALID_TIME;

// Structure to hold schedule entry data
struct ScheduleEntry {
    int startHour;
    int startMinute;
    int endHour;
    int endMinute;
    int intervalMinutes;
};

// Parse sleep interval from header value
// Supports simple formats like "20s", "5m", "2h", "1d"
// Also supports schedule format: "00:00-06:00=480 06:00-18:00=15 18:00-24:00=30"
// timezoneOffset: offset in hours from UTC (e.g., -5 for EST, +5.5 for IST) to interpret schedule times in local time
int parseSleepInterval(String headerValue, float timezoneOffset = 0);

// Parse individual schedule entry (format: HH:MM-HH:MM=MINUTES)
bool parseScheduleEntry(String entry, ScheduleEntry &result);

// Check if current time is within a schedule entry's time range
bool isTimeInRange(int currentHour, int currentMinute, const ScheduleEntry &entry);
