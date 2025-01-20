#pragma once

#include <ArduinoJson.h>
#include "Inkplate.h"
#include "FreeSans18pt7b.h"
#include "globals.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>

#include "network.h"

Network network;

class Draw
{

public:
    void update(Inkplate &d, String serverAddress)
    {
        Serial.println("Updating display with address:");
        Serial.println(serverAddress);

        if (serverAddress.length() == 0)
        {
            Serial.println("Error: serverAddress is empty");
            return;
        }

        HTTPClient http;

        Serial.println("[HTTP] begin...\n");

        // Your Domain name with URL path or IP address with path
        if (!http.begin(serverAddress))
        {
            Serial.println("Error: Failed to begin HTTP connection");
            return;
        }

        char targetHeaderName[] = "Content-Type";
        const char *headerKeys[] = {targetHeaderName};
        const size_t headerKeysCount = sizeof(headerKeys) / sizeof(headerKeys[0]);
        http.collectHeaders(headerKeys, headerKeysCount);
        Serial.println("[HTTP] GET...\n");

        int httpResponseCode = http.GET();
        if (httpResponseCode <= 0)
        {
            Serial.printf("Error: HTTP GET request failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
            http.end();
            return;
        }

        Serial.printf("HTTP Response code: %d\n", httpResponseCode);
        Serial.println(http.header(targetHeaderName));
        String contentType = http.header(targetHeaderName);
        bool shouldUpdateScreen = false;

        // if (httpResponseCode == 200)
        // {
        //     if (contentType == "image/png")
        //     {
        //         int32_t len = http.getSize();
        //         if (!d.drawPngFromWeb(http.getStreamPtr(), 0, 0, len, 0, 0))
        //         {
        //             drawCentreString(d, String("Image open error: ") + String(serverAddress));
        //         }
        //         else
        //         {
        //             shouldUpdateScreen = true;
        //         }
        //     }
        //     else
        //     {
        //         if (globals::lastState != 2)
        //         {
        //             drawCentreString(d, String("Sleeping"));
        //             globals::lastState = 2;
        //             shouldUpdateScreen = true;
        //         }
        //     }
        // }
        // else
        // {

        //     Serial.println("RESPONSE CODE IS NOT 200");
        //     Serial.println(httpResponseCode);

        //     if (globals::lastState != 1)
        //     {
        //         drawCentreString(d, String("can't connect to server: ") + String(serverAddress));
        //         globals::lastState = 1;
        //         shouldUpdateScreen = true;
        //     }
        // }

        // if (shouldUpdateScreen)
        // {
        //     d.display();
        // }
    }

    void drawCentreString(Inkplate &d, String buf)
    {
        int16_t x1, y1;
        uint16_t w, h;
        d.setTextSize(3);
        d.setTextColor(7, 1);
        d.getTextBounds(buf, 600, 790, &x1, &y1, &w, &h); // calc width of new string
        d.setCursor(600 - w / 2, 790);
        d.println(buf);
        d.display();
    }
    /* void drawImage(Inkplate &d)
    {
        SdFile file;
        uint8_t month = d.rtcGetMonth() + 1;
        uint8_t day = d.rtcGetDay();

        char *imageAddress;
        imageAddress = new char[60];
        imageAddress = getImageAddress(imageAddress, month, day, globals::imageIndex);

        if (!d.sdCardInit())
        {
            Serial.println("SD Card NOT ok! ");
            // drawErrorMessage(line1, line2, line3)
            return;
        }

        if (file.open(imageAddress, O_RDONLY))
        {
            Serial.println("file exists");
        }
        else
        {
            Serial.println("file does not exist");
            Serial.println();
            d.println("file does not exist");
            globals::imageIndex = 0;
            imageAddress = getImageAddress(imageAddress, month, day, globals::imageIndex);
        }

        d.drawImage(imageAddress, 0, 0, 0, 1);
        globals::imageIndex++;
    } */

    /* void drawRedditPosts(Inkplate &d)
    {
        HTTPClient http;
        DynamicJsonDocument doc(2000);

        d.setCursor(0, 50);
        d.setFont(&FreeSans18pt7b);
        d.setTextColor(0, 7);

        if (network.connect())
        {
            int httpResponseCode = network.setupClientForStream(http, reddit_listings_url);

            if (httpResponseCode > 0)
            {
                DeserializationError error = deserializeJson(doc, http.getStream());
                if (error)
                {
                    Serial.print("deserializeJson() failed: ");
                    // TODO print error to screen
                    return;
                }
                for (JsonObject item : doc.as<JsonArray>())
                {
                    const char *created = item["created"];

                    d.println(item["title"].as<const char *>());
                }
            }
            else
            {
                d.print("Error displaying reddit posts, code: ");
                d.println(httpResponseCode);
            }
            http.end();
        }
        else
        {
            Serial.println("WiFi Disconnected");
        }
    } */
};