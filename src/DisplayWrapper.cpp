#include "DisplayWrapper.h"
#include <HTTPClient.h>
#include <WiFiClient.h>

#if defined(BOARD_GxEPD2)
// TODO fix ths pins
DisplayWrapper::DisplayWrapper() : d(GxEPD2_583_T8(2, 0, 2, 4)) // Initialize the display object with correct parameters
{
}
#elif defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
DisplayWrapper::DisplayWrapper() : d(DISPLAY_CLASS(INKPLATE_3BIT)) // Initialize the display object for Inkplate
{
}
#else
#error "Unsupported board selection. Please define BOARD_GxEPD2 or ARDUINO_INKPLATE10/ARDUINO_INKPLATE10V2."
#endif

void DisplayWrapper::begin()
{
#if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
    // display.setIntOutput(pin, state, level, enable, addr);
    d.begin();
#else
    // GxEPD2 does not support this method
#endif
}

bool DisplayWrapper::sdCardInit()
{
#if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
    return d.sdCardInit();
#else
    // GxEPD2 does not support this method
    return false;
#endif
}

void DisplayWrapper::sdCardSleep()
{
#if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
    display.sdCardSleep();
#else
    // GxEPD2 does not support this method
#endif
}

void DisplayWrapper::clearDisplay()
{
#if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
    d.clearDisplay();
#else
    d.fillScreen(0);
#endif
}

void DisplayWrapper::display()
{
    d.display();
}

void DisplayWrapper::setTextSize(uint8_t size)
{
    d.setTextSize(size);
}

void DisplayWrapper::setFont(const GFXfont *font)
{
    d.setFont(font);
}

void DisplayWrapper::setTextColor(uint16_t color)
{
    d.setTextColor(color);
}

void DisplayWrapper::setCursor(int16_t x, int16_t y)
{
    d.setCursor(x, y);
}

void DisplayWrapper::println(const String &text)
{
    d.println(text);
}

void DisplayWrapper::getTextBounds(const String &str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h)
{
    d.getTextBounds(str.c_str(), x, y, x1, y1, w, h);
}

int16_t DisplayWrapper::width()
{
    return d.width();
}

int16_t DisplayWrapper::height()
{
    return d.height();
}

bool DisplayWrapper::readTouchpad(uint8_t pad)
{
#if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
    return d.readTouchpad(pad);
#else
    // GxEPD2 does not support this method
    return false;
#endif
}

void DisplayWrapper::getStringCenter(String buf, int *a, int *b)
{
    int16_t x1, y1;
    uint16_t w, h;

    d.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
    *a = (d.width() - w) / 2;
    *b = (d.height() - h) / 2;
}

void DisplayWrapper::drawErrorMessage(String message)
{
    d.setTextSize(1);
    d.setFont(&FreeSans24pt7b);
    d.setTextColor(0xFFFF);
    int centerX;
    int centerY;
    getStringCenter(message, &centerX, &centerY);

    d.setCursor(centerX, centerY);

    d.println(message);
}

void DisplayWrapper::drawDebugInfo(Config &config)
{
    log("Displaying debug info");
    d.setTextSize(1);
    d.setFont(&FreeSans9pt7b);
    d.setTextColor(0xFFFF);
    int centerX;
    int centerY;
    String debugString = "server: " + String(config.server) + " | ssid: " + config.ssid + " | sleep(secs): " + String(config.sleepTime);
    getStringCenter(debugString, &centerX, &centerY);

    d.setCursor(centerX, 810);

    d.println(debugString);
}

void DisplayWrapper::drawImage(const char *server)
{
    log("Will display image now");
#if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
    if (!d.drawImage(server, 0, 0, 0, true))
    {
        log("Image open error");
        drawErrorMessage("Error: Could not draw image");
    }
    log("Image displayed");
#else
// GxEPD2 does not support this method
#endif
}

#if defined(BOARD_GxEPD2)
// Callback function for PNG decoding (specific to GxEPD2)
static void pngDraw(PNGDRAW *pDraw)
{
    // Get the display object from the user pointer
    GxEPD2_BW<GxEPD2_583_T8, GxEPD2_583_T8::HEIGHT> *display = (GxEPD2_BW<GxEPD2_583_T8, GxEPD2_583_T8::HEIGHT> *)pDraw->pUser;

    // Iterate over the pixels in the current row
    for (int x = 0; x < pDraw->iWidth; x++)
    {
        uint8_t pixelIndex = pDraw->pPixels[x];       // Get the pixel index
        uint16_t color = pDraw->pPalette[pixelIndex]; // Get the color from the palette
        // TODO this is probably wrong
        display->drawPixel(x, pDraw->y, color); // Draw the pixel
    }
}
#endif

bool DisplayWrapper::drawImage(const char *url, int x, int y, uint8_t max_width, bool dither)
{
#if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
    // Inkplate implementation
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
        WiFiClient *stream = http.getStreamPtr();
        if (!d.drawPngFromWeb(stream, x, y, max_width, dither))
        {
            log("Failed to draw PNG on Inkplate");
            return false;
        }
        return true;
    }
    else
    {
        log("HTTP error while fetching PNG for Inkplate");
        return false;
    }
#elif defined(BOARD_GxEPD2)
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
        WiFiClient *stream = http.getStreamPtr();
        PNG png;
        // int rc = png.openRAM(stream, 100, pngDraw);
        // Read data from the stream into a buffer
        uint8_t *buffer = new uint8_t[100]; // Adjust size as needed
        stream->readBytes(reinterpret_cast<char *>(buffer), 100);

        if (png.openRAM(buffer, 100, pngDraw))
            png.close();
        delete[] buffer; // Free the allocated memory
        // png.setUser(&d); // Pass the display object to the PNG decoder
        if (png.decode(NULL, 0))
        {
            log("Failed to decode PNG for GxEPD2");
            delete[] buffer; // Free the allocated memory
            return true;
        }
        png.close();
    }
    else
    {
        log("HTTP error while fetching PNG for GxEPD2");
        return false;
    }
#else
    // Unsupported board
    log("Unsupported board for drawImage");
    return false;
#endif
    return false; // Ensure all code paths return a value
}

// bool DisplayWrapper::drawPngFromWeb(const char *url, int x, int y, uint8_t max_width, bool dither)
// {
// #if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
//     return display.drawPngFromWeb(url, x, y, max_width, dither);
// #else
//     // GxEPD2 does not support this method
//     return false;
// #endif
// }

// bool DisplayWrapper::drawPngFromWeb(Stream *stream, int x, int y, int32_t len, bool dither, bool flip)
// {
// #if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
//     return display.drawPngFromWeb(stream, x, y, len, dither, flip);
// #else
//     // GxEPD2 does not support this method
//     return false;
// #endif
// }

// bool DisplayWrapper::drawPngFromWebGxEPD2(const char *url, int x, int y)
// {
// #if defined(BOARD_GxEPD2)
//     HTTPClient http;
//     http.begin(url);
//     int httpCode = http.GET();
//     if (httpCode == HTTP_CODE_OK)
//     {
//         WiFiClient *stream = http.getStreamPtr();
//         PNG png;
//         if (png.open(stream, pngDraw))
//         {
//             if (png.decode(NULL, 0))
//             {
//                 png.close();
//                 return true;
//             }
//             png.close();
//         }
//     }
//     http.end();
//     return false;
// #else
//     // Inkplate does not support this method
//     return false;
// #endif
// }

// bool DisplayWrapper::drawImage(const char *url, int x, int y, uint8_t max_width, bool dither)
// {
// #if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
//     return drawPngFromWeb(url, x, y, max_width, dither);
// #elif defined(BOARD_GxEPD2)
//     return drawPngFromWebGxEPD2(url, x, y);
// #else
//     // Unsupported board
//     return false;
// #endif
// }

// bool DisplayWrapper::drawImageFromClient(HTTPClient &httpClient, int32_t len)
// {
//     log("Will display image now");

//     if (!display.drawImage(httpClient.getStreamPtr(), 0, 0, len, true, false))
//     {
//         log("Image open error");
//         // drawErrorMessage(d, "Error: Could not draw image");
//         return false;
//     }
//     log("Image displayed");

//     return true;
// }

// uint16_t DisplayWrapper::pngDraw(PNGDRAW *pDraw)
// {
// #if defined(BOARD_GxEPD2)
//     uint16_t usPixels[320]; // 1 line of pixels
//     PNG *pPng = (PNG *)pDraw->pUser;
//     int iPitch = pDraw->iWidth;
//     int y = pDraw->y;
//     int x = pDraw->x;
//     pPng->getLineAsRGB565(y, usPixels, iPitch);
//     display.drawRGBBitmap(x, y, usPixels, pDraw->iWidth, 1);
// #endif
//     return 0;
// }