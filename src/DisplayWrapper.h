#ifndef DISPLAYWRAPPER_H
#define DISPLAYWRAPPER_H

#include "config.h"
#include "global.h"

#if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
#include "Inkplate.h"
#define DISPLAY_CLASS Inkplate
#elif defined(BOARD_GxEPD2)
#include <GxEPD2_BW.h>
#include <PNGdec.h>
#define DISPLAY_CLASS GxEPD2_BW<GxEPD2_213>
#else
#error "Unsupported board selection."
#endif

class DisplayWrapper
{
public:
    DisplayWrapper();

    //     void begin();
    //     bool sdCardInit();
    //     void setIntOutput(uint8_t pin, bool state, bool level, bool enable, uint8_t addr);
    //     void setIntPin(uint8_t pad, int level, uint8_t addr);
    //     void clearDisplay();
    //     void display();
    //     void setTextSize(uint8_t size);
    //     void setFont(const GFXfont *font);
    //     void setTextColor(uint16_t color);
    //     void setCursor(int16_t x, int16_t y);
    //     void println(const String &text);
    //     void getTextBounds(const String &str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
    //     int16_t width() const;
    //     int16_t height() const;
    //     bool drawPngFromWeb(const char *url, int x, int y, uint8_t max_width, bool dither);
    //     bool drawPngFromWeb(Stream *stream, int x, int y, int32_t len, bool dither, bool flip);
    //     bool drawPngFromWebGxEPD2(const char *url, int x, int y);
    //     bool drawImage(const char *url, int x, int y, uint8_t max_width, bool dither);
    //     bool readTouchpad(uint8_t pad);
    //     void sdCardSleep();

private:
    DISPLAY_CLASS display;
    // static uint16_t pngDraw(PNGDRAW *pDraw);
};

#endif // DISPLAYWRAPPER_H