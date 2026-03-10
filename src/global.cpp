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
    Config tempConfig = {}; // Create temp config for stopProgram
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
        drawSleepStatus(d, time, config.timezoneOffset);
        d.display();
    }
    
    log("Going to sleep for " + String(time) + " seconds");
    Serial.flush();
    esp_deep_sleep_start(); // Put ESP32 into deep sleep. Program stops here.
}