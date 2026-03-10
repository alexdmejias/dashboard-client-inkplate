#include "draw.h"
#include "fonts/FreeSans9pt7b.h"
#include "fonts/FreeSans12pt7b.h"
#include "fonts/FreeSans24pt7b.h"

void getStringCenter(Inkplate &d, String buf, int *a, int *b)
{
    int16_t x1, y1;
    uint16_t w, h;

    d.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
    *a = (d.width() - w) / 2;
    *b = (d.height() - h) / 2;
}

void drawErrorMessage(Inkplate &d, String message)
{
    d.setTextSize(1);
    d.setFont(&FreeSans24pt7b);
    d.setTextColor(WHITE, BLACK);
    int centerX;
    int centerY;
    getStringCenter(d, message, &centerX, &centerY);

    d.setCursor(centerX, centerY);

    d.println(message);
}

void drawSleepStatus(Inkplate &d, int sleepDuration, float timezoneOffset)
{
    d.clearDisplay();
    d.setTextSize(1);
    d.setTextColor(WHITE, BLACK);
    
    // Get current time
    time_t now = time(nullptr);
    
    // Apply timezone offset
    time_t localNow = now + (int)(timezoneOffset * 3600);
    time_t wakeTime = localNow + sleepDuration;
    
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
    if (sleepDuration >= 86400) {
        int days = sleepDuration / 86400;
        int hours = (sleepDuration % 86400) / 3600;
        int minutes = (sleepDuration % 3600) / 60;
        durationStr = String(days) + "d " + String(hours) + "h " + String(minutes) + "m";
    } else if (sleepDuration >= 3600) {
        int hours = sleepDuration / 3600;
        int minutes = (sleepDuration % 3600) / 60;
        durationStr = String(hours) + "h " + String(minutes) + "m";
    } else if (sleepDuration >= 60) {
        int minutes = sleepDuration / 60;
        int seconds = sleepDuration % 60;
        durationStr = String(minutes) + "m " + String(seconds) + "s";
    } else {
        durationStr = String(sleepDuration) + "s";
    }
    
    // Draw title
    d.setFont(&FreeSans24pt7b);
    int x, y;
    String title = "Going to Sleep";
    getStringCenter(d, title, &x, &y);
    d.setCursor(x, 200);
    d.println(title);
    
    // Draw sleep details
    d.setFont(&FreeSans12pt7b);
    
    // Current time
    String tzLabel = "UTC";
    if (timezoneOffset != 0) {
        tzLabel = "UTC" + String(timezoneOffset >= 0 ? "+" : "") + String(timezoneOffset, 1);
    }
    String sleepMsg = "Sleep Time: " + String(sleepTimeStr) + " " + tzLabel;
    getStringCenter(d, sleepMsg, &x, &y);
    d.setCursor(x, 300);
    d.println(sleepMsg);
    
    // Duration
    String durationMsg = "Duration: " + durationStr;
    getStringCenter(d, durationMsg, &x, &y);
    d.setCursor(x, 350);
    d.println(durationMsg);
    
    // Wake time
    String wakeMsg = "Wake Time: " + String(wakeTimeStr) + " " + tzLabel;
    getStringCenter(d, wakeMsg, &x, &y);
    d.setCursor(x, 400);
    d.println(wakeMsg);
    
    // Wake-up instruction
    d.setFont(&FreeSans9pt7b);
    String instruction = "Touch any pad to wake early";
    getStringCenter(d, instruction, &x, &y);
    d.setCursor(x, 500);
    d.println(instruction);
}

void drawDebugInfo(Inkplate &d, Config &config)
{
    // log("Displaying debug info");
    log("Displaying debug info");
    d.setTextSize(1);
    d.setFont(&FreeSans9pt7b);
    d.setTextColor(WHITE, BLACK);
    int centerX;
    int centerY;
    String debugString = "server: " + String(config.server) + " | ssid: " + config.ssid + " | sleepTime(secs): " + String(config.sleepTime);
    getStringCenter(d, debugString, &centerX, &centerY);

    d.setCursor(centerX, 810);

    d.println(debugString);
}

void drawImage(Inkplate &d, const char *server)
{
    log("Will display image now");

    if (!d.drawPngFromWeb(server, 0, 0, 0, true))
    {
        log("Image open error");
        drawErrorMessage(d, "Error: Could not draw image");
    }
    log("Image displayed");
}

bool drawImageFromClient(Inkplate &d, HTTPClient &httpClient, int32_t len)
{
    log("Will display image now");

    if (!d.drawPngFromWeb(httpClient.getStreamPtr(), 0, 0, len, true, false))
    {
        log("Image open error");
        // drawErrorMessage(d, "Error: Could not draw image");
        return false;
    }
    log("Image displayed");

    return true;
}