#include "global.h"
#include "draw.h"
#include "esp_sleep.h"
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
    tempConfig.wakeButtonPin = 36; // Use default GPIO 36 for wake button
    handleSleep(d, 3001, tempConfig);
}

void handleSleep(Inkplate &d, int time, Config &config)
{
    // Enable timer wake-up
    esp_sleep_enable_timer_wakeup(time * uS_TO_S_FACTOR); // Activate wake-up timer
    
    // Enable touchpad wake-up for manual wake capability
    esp_sleep_enable_ext1_wakeup(TOUCHPAD_WAKE_MASK, ESP_EXT1_WAKEUP_ANY_HIGH);
    
    // Enable external button wake-up on the specified GPIO pin
    // The button should connect the pin to GND, so we wake on LOW (0)
    if (config.wakeButtonPin >= 0) {
        // Validate that the pin supports EXT0 wake-up
        // Valid pins for EXT0: 0, 2, 4, 12-15, 25-27, 32-39
        bool validPin = (config.wakeButtonPin == 0 || config.wakeButtonPin == 2 || config.wakeButtonPin == 4 ||
                        (config.wakeButtonPin >= 12 && config.wakeButtonPin <= 15) ||
                        (config.wakeButtonPin >= 25 && config.wakeButtonPin <= 27) ||
                        (config.wakeButtonPin >= 32 && config.wakeButtonPin <= 39));
        
        if (validPin) {
            esp_sleep_enable_ext0_wakeup((gpio_num_t)config.wakeButtonPin, 0);
            log("External button wake-up enabled on GPIO " + String(config.wakeButtonPin));
        } else {
            log("Warning: GPIO " + String(config.wakeButtonPin) + " does not support EXT0 wake-up. Valid pins: 0, 2, 4, 12-15, 25-27, 32-39");
        }
    }
    
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
        
        // Use wake-up hint based on what's configured
        String wakeHint = "Touch pad or button to wake";
        
        drawErrorMessage(d, sleepMessage, wakeHint);
        d.display();
    }
    
    log("Going to sleep for " + String(time) + " seconds");
    Serial.flush();
    esp_deep_sleep_start(); // Put ESP32 into deep sleep. Program stops here.
}
