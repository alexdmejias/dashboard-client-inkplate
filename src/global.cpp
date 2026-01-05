#include "global.h"
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
    handleSleep(3001, 36); // Use default GPIO 36 for wake button
}

void handleSleep(int time, int wakeButtonPin)
{
    esp_sleep_enable_timer_wakeup(time * uS_TO_S_FACTOR); // Activate wake-up timer
    
    // Enable external button wake-up on the specified GPIO pin
    // The button should connect the pin to GND, so we wake on LOW (0)
    if (wakeButtonPin >= 0) {
        // Validate that the pin supports EXT0 wake-up
        // Valid pins for EXT0: 0, 2, 4, 12-15, 25-27, 32-39
        bool validPin = (wakeButtonPin == 0 || wakeButtonPin == 2 || wakeButtonPin == 4 ||
                        (wakeButtonPin >= 12 && wakeButtonPin <= 15) ||
                        (wakeButtonPin >= 25 && wakeButtonPin <= 27) ||
                        (wakeButtonPin >= 32 && wakeButtonPin <= 39));
        
        if (validPin) {
            esp_sleep_enable_ext0_wakeup((gpio_num_t)wakeButtonPin, 0);
            log("External button wake-up enabled on GPIO " + String(wakeButtonPin));
        } else {
            log("Warning: GPIO " + String(wakeButtonPin) + " does not support EXT0 wake-up. Valid pins: 0, 2, 4, 12-15, 25-27, 32-39");
        }
    }
    
    log("Going to sleep for " + String(time) + " seconds");
    Serial.flush();
    esp_deep_sleep_start(); // Put ESP32 into deep sleep. Program stops here.
}