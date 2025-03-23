#include "DisplayWrapper.h"
// #include <HTTPClient.h>

// DisplayWrapper::DisplayWrapper() : display() {}

void DisplayWrapper::begin()
{
    display.begin();
}

// bool DisplayWrapper::sdCardInit()
// {
//     return display.sdCardInit();
// }

// void DisplayWrapper::setIntOutput(uint8_t pin, bool state, bool level, bool enable, uint8_t addr)
// {
// #if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
//     display.setIntOutput(pin, state, level, enable, addr);
// #else
//     // GxEPD2 does not support this method
// #endif
// }

// void DisplayWrapper::setIntPin(uint8_t pad, int level, uint8_t addr)
// {
// #if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
//     display.setIntPin(pad, level, addr);
// #else
//     // GxEPD2 does not support this method
// #endif
// }

// void DisplayWrapper::clearDisplay()
// {
//     display.clearDisplay();
// }

// void DisplayWrapper::display()
// {
//     display.display();
// }

// void DisplayWrapper::setTextSize(uint8_t size)
// {
//     display.setTextSize(size);
// }

// void DisplayWrapper::setFont(const GFXfont *font)
// {
//     display.setFont(font);
// }

// void DisplayWrapper::setTextColor(uint16_t color)
// {
//     display.setTextColor(color);
// }

// void DisplayWrapper::setCursor(int16_t x, int16_t y)
// {
//     display.setCursor(x, y);
// }

// void DisplayWrapper::println(const String &text)
// {
//     display.println(text);
// }

// void DisplayWrapper::getTextBounds(const String &str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h)
// {
//     display.getTextBounds(str.c_str(), x, y, x1, y1, w, h);
// }

// int16_t DisplayWrapper::width() const
// {
//     return display.width();
// }

// int16_t DisplayWrapper::height() const
// {
//     return display.height();
// }

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

// void sdCardSleep()
// {
// #if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
//     display.sdCardSleep();
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

// bool DisplayWrapper::readTouchpad(uint8_t pad)
// {
// #if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
//     return display.readTouchpad(pad);
// #else
//     // GxEPD2 does not support this method
//     return false;
// #endif
// }

// void DisplayWrapper::getStringCenter(String buf, int *a, int *b)
// {
//     int16_t x1, y1;
//     uint16_t w, h;

//     display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
//     *a = (display.width() - w) / 2;
//     *b = (display.height() - h) / 2;
// }

// void DisplayWrapper::drawErrorMessage(String message)
// {
//     display.setTextSize(1);
//     display.setFont(&FreeSans24pt7b);
//     display.setTextColor(WHITE);
//     int centerX;
//     int centerY;
//     getStringCenter(d, message, &centerX, &centerY);

//     display.setCursor(centerX, centerY);

//     display.println(message);
// }

// void DisplayWrapper::drawDebugInfo(Config &config)
// {
//     // log("Displaying debug info");
//     log("Displaying debug info");
//     display.setTextSize(1);
//     display.setFont(&FreeSans9pt7b);
//     display.setTextColor(WHITE);
//     int centerX;
//     int centerY;
//     String debugString = "server: " + String(config.server) + " | ssid: " + config.ssid + " | sleep(secs): " + String(config.sleepTime);
//     getStringCenter(d, debugString, &centerX, &centerY);

//     display.setCursor(centerX, 810);

//     display.println(debugString);
// }

// void DisplayWrapper::drawImage(const char *server)
// {
//     log("Will display image now");

//     if (!display.drawImage(server, 0, 0, 0, true))
//     {
//         log("Image open error");
//         drawErrorMessage(d, "Error: Could not draw image");
//     }
//     log("Image displayed");
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