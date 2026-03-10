#include "global.h"
#include "draw.h"
#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */

void log(String msg)
{
    Serial.println("::::::::::: " + String(msg));
}

void stopProgram(Inkplate &d)
{
    d.display();
    d.sdCardSleep();
    // Use empty config (showSleepStatus=false) - don't show sleep status during error conditions
    Config tempConfig = {};
    handleSleep(d, 3001, tempConfig);
}

void handleSleep(Inkplate &d, int time, Config &config)
{
    // Enable touchpad wake-up for manual wake capability
    esp_sleep_enable_ext1_wakeup(TOUCHPAD_WAKE_MASK, ESP_EXT1_WAKEUP_ANY_HIGH);
    
    // Enable timer wake-up
    esp_sleep_enable_timer_wakeup(time * uS_TO_S_FACTOR); // Activate wake-up timer
    
    // Display sleep status if enabled
    if (config.showSleepStatus && time > 0) {
        d.clearDisplay();
        
        // Get current time
        time_t now = time(nullptr);
        
        // Apply timezone offset
        time_t localNow = now + (int)(config.timezoneOffset * 3600);
        time_t wakeTime = localNow + time;
        
        struct tm sleepTimeInfo;
        struct tm wakeTimeInfo;
        gmtime_r(&localNow, &sleepTimeInfo);
        gmtime_r(&wakeTime, &wakeTimeInfo);
        
        // Format times
        char sleepTimeStr[30];
        char wakeTimeStr[30];
        strftime(sleepTimeStr, sizeof(sleepTimeStr), "%Y-%m-%d %H:%M:%S", &sleepTimeInfo);
        strftime(wakeTimeStr, sizeof(wakeTimeStr), "%Y-%m-%d %H:%M:%S", &wakeTimeInfo);
        
        // Format duration in human-readable form
        String durationStr;
        if (time >= 86400) {
            int days = time / 86400;
            int hours = (time % 86400) / 3600;
            int minutes = (time % 3600) / 60;
            durationStr = String(days) + "d " + String(hours) + "h " + String(minutes) + "m";
        } else if (time >= 3600) {
            int hours = time / 3600;
            int minutes = (time % 3600) / 60;
            durationStr = String(hours) + "h " + String(minutes) + "m";
        } else if (time >= 60) {
            int minutes = time / 60;
            int seconds = time % 60;
            durationStr = String(minutes) + "m " + String(seconds) + "s";
        } else {
            durationStr = String(time) + "s";
        }
        
        // Build timezone label
        String tzLabel = "UTC";
        if (config.timezoneOffset != 0) {
            tzLabel = "UTC" + String(config.timezoneOffset >= 0 ? "+" : "") + String(config.timezoneOffset, 1);
        }
        
        // Create sleep status message
        String sleepMessage = "Going to Sleep\n" + 
                             String(sleepTimeStr) + " " + tzLabel + "\n" +
                             "for " + durationStr + "\n" +
                             "Wake: " + String(wakeTimeStr);
        
        drawErrorMessage(d, sleepMessage);
        d.display();
    }
    
    log("Going to sleep for " + String(time) + " seconds");
    Serial.flush();
    esp_deep_sleep_start(); // Put ESP32 into deep sleep. Program stops here.
}