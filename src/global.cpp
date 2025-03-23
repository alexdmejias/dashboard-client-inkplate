#include "global.h"
#include <Arduino.h>

void log(String msg)
{
    Serial.println("::::::::::: " + String(msg));
}

void stopProgram(DisplayWrapper &d)
{
    // d.display();
    // d.sdCardSleep();
    handleSleep(3001);
}

void handleSleep(int time)
{
    esp_sleep_enable_timer_wakeup(time * uS_TO_S_FACTOR); // Activate wake-up timer
    esp_sleep_enable_ext1_wakeup(TOUCHPAD_WAKE_MASK, ESP_EXT1_WAKEUP_ANY_HIGH);
    log("Going to sleep for " + String(time) + " seconds");
    Serial.flush();
    esp_deep_sleep_start(); // Put ESP32 into deep sleep. Program stops here.
}