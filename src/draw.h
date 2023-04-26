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
    void update(Inkplate &d, WiFiManager &wm, const char *value)
    {
        Serial.println("------v");
        // Serial.println(serverAddress);
        Serial.println("------^");

        d.setTextSize(3);
        d.setTextColor(0, 7);
        d.setCursor(100, 360);

        // WiFi.mode(WIFI_MODE_STA);
        // WiFi.begin(wm.getWiFiSSID().c_str(), wm.getWiFiPass().c_str());
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
            d.partialUpdate();
        }
        Serial.println("\nWiFi OK! Downloading...");

        Serial.println("connected");
        Serial.println("value:");
        Serial.print(value);

        // if (!d.drawImage("http://192.168.0.97:3000", 0, 100, true, false))
        delay(5000);
        Serial.println("delay done");
        if (!d.drawImage("http://192.168.0.97:3000", d.PNG, 0, 0))
        {
            d.println("Image open error");
        }
        d.display();

        // if (!canDrawImage)
        // {
        //     d.setTextSize(3);
        //     d.setTextColor(0, 7);
        //     d.setCursor(100, 360);
        //     d.println("Can't connect to the server attempting to connect to ");
        //     d.setCursor(100, 390);
        //     d.println(serverAddress);
        // }

        d.display();

        /* if (globals::refreshIndex == 0)
        {
            drawImage(d);
        }
        else if (globals::refreshIndex == 1)
        {
            drawBoxes(d);
        }
        else if (globals::refreshIndex == 2)
        {
            drawRedditPosts(d);
        }

        d.display();
        globals::refreshIndex++;

        if (globals::refreshIndex > 2)
        {
            globals::refreshIndex = 0;
        } */
    }

private:
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