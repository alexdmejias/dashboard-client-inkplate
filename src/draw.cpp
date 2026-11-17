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

// Wrapper class around WiFiClient to protect against infinite loops in Inkplate's
// downloadFile(WiFiClient *s, int32_t len) if the socket disconnects or stalls mid-transfer.
class SafeWiFiClientWrapper : public WiFiClient
{
private:
    WiFiClient *_client;
    unsigned long _timeoutMs;
    unsigned long _lastReadTime;
    bool _timedOut;
    int32_t _remaining;

public:
    SafeWiFiClientWrapper(WiFiClient *client, unsigned long timeoutMs, int32_t expectedLen)
        : _client(client),
          _timeoutMs(timeoutMs),
          _lastReadTime(millis()),
          _timedOut(false),
          _remaining(expectedLen)
    {
    }

    bool hasTimedOut() const
    {
        return _timedOut;
    }

    int available() override
    {
        if (_timedOut)
        {
            // Drain remaining bytes immediately so downloadFile's while (len > 0) loop exits cleanly
            return (_remaining > 0) ? ((_remaining > 128) ? 128 : _remaining) : 0;
        }

        if (!_client)
        {
            _timedOut = true;
            return (_remaining > 0) ? ((_remaining > 128) ? 128 : _remaining) : 0;
        }

        int avail = _client->available();
        if (avail > 0)
        {
            _lastReadTime = millis();
            return avail;
        }

        // Check if socket was closed or read has timed out
        if (!_client->connected() || (millis() - _lastReadTime > _timeoutMs))
        {
            log("SafeWiFiClientWrapper: Socket closed or timeout reading image stream!");
            _timedOut = true;
            return (_remaining > 0) ? ((_remaining > 128) ? 128 : _remaining) : 0;
        }

        return 0;
    }

    int read() override
    {
        if (_timedOut)
        {
            if (_remaining > 0)
                _remaining--;
            return 0;
        }
        if (!_client)
            return -1;
        int b = _client->read();
        if (b >= 0)
        {
            _lastReadTime = millis();
            if (_remaining > 0)
                _remaining--;
        }
        return b;
    }

    int read(uint8_t *buf, size_t size) override
    {
        if (_timedOut)
        {
            size_t toDrain = (_remaining > 0) ? min((size_t)_remaining, size) : 0;
            if (toDrain > 0)
            {
                memset(buf, 0, toDrain);
                _remaining -= toDrain;
                return toDrain;
            }
            return 0;
        }
        if (!_client)
            return -1;
        int bytesRead = _client->read(buf, size);
        if (bytesRead > 0)
        {
            _lastReadTime = millis();
            if (_remaining > 0)
                _remaining -= bytesRead;
        }
        return bytesRead;
    }

    uint8_t connected() override
    {
        return (_client && !_timedOut) ? _client->connected() : 0;
    }

    void stop() override
    {
        if (_client)
            _client->stop();
    }
};

bool drawImageFromClient(Inkplate &d, HTTPClient &httpClient, int32_t len, int httpTimeout)
{
    log("Will display image now");

    if (len <= 0)
    {
        log("Invalid image length: " + String(len));
        return false;
    }

    unsigned long timeoutMs = (httpTimeout > 0) ? (static_cast<unsigned long>(httpTimeout) * 1000UL) : 15000UL;
    WiFiClient *rawClient = httpClient.getStreamPtr();
    SafeWiFiClientWrapper safeClient(rawClient, timeoutMs, len);

    bool result = d.drawPngFromWeb(&safeClient, 0, 0, len, true, false);

    if (safeClient.hasTimedOut())
    {
        log("Image download failed due to timeout or socket disconnect");
        return false;
    }

    if (!result)
    {
        log("Image open error (corrupt or unsupported format)");
        return false;
    }

    log("Image displayed");
    return true;
}