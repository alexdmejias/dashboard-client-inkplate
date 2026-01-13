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

void drawErrorMessage(Inkplate &d, String message, String debugHint)
{
    d.setTextSize(1);
    d.setFont(&FreeSans24pt7b);
    d.setTextColor(WHITE, BLACK);
    int centerX;
    int centerY;
    getStringCenter(d, message, &centerX, &centerY);

    d.setCursor(centerX, centerY);
    d.println(message);

    // Display debug hint if provided
    if (debugHint.length() > 0)
    {
        d.setFont(&FreeSans12pt7b);
        int hintCenterX;
        int hintCenterY;
        getStringCenter(d, debugHint, &hintCenterX, &hintCenterY);
        const int DEBUG_HINT_VERTICAL_OFFSET = 60;
        d.setCursor(hintCenterX, centerY + DEBUG_HINT_VERTICAL_OFFSET);
        d.println(debugHint);
    }
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
        drawErrorMessage(d, "Error: Could not draw image", "Check server is returning valid PNG");
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