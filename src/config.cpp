#include "config.h"

Config defaultConfig = {
    "example.com",           // server
    "fake_password",         // password
    "my_home_network-5g",    // ssid
    30,                      // wifiTimeout
    20,                      // sleepTime
    true,                    // debug
    "EST5EDT,M3.2.0,M11.1.0" // timezone
};

int MAX_CONFIG_SIZE = 1000;

String readUserInput(const char *prompt, unsigned long timeout);

void readConfig(Inkplate &d, const char *filename, Config &config)
{
    SdFile file;
    // Init SD card. Display if SD card is init propery or not.
    if (d.sdCardInit())
    {
        log("SD Card ok! Reading data...");
        // Allocate a temporary JsonDocument
        JsonDocument doc;

        // Try to load text with max lenght of 200 chars.
        if (!file.open(filename, O_RDONLY))
        { // If it fails to open, send error message to display, otherwise read the file.
            log("File open error");
            drawErrorMessage(d, "Error: Could not open file");
            stopProgram(d);
        }
        else
        {
            char text[MAX_CONFIG_SIZE + 1]; // Array where data from SD card is stored (max 500 chars here)
            int len = file.fileSize();      // Read how big is file that we are opening
            if (len > MAX_CONFIG_SIZE)
            {
                len = MAX_CONFIG_SIZE; // If it's more than 200 bytes (200 chars), limit to max 200 bytes
            }
            file.read(text, len); // Read data from file and save it in text array
            text[len] = 0;        // Put null terminating char at the and of data

            DeserializationError error = deserializeJson(doc, text);
            if (error)
            {
                log("Failed to read file, using default configuration" + String(error.c_str()));
            }

            log("Copying values to config object");
            if (!doc["server"].is<const char *>() || !doc["ssid"].is<const char *>() || !doc["password"].is<const char *>())
            {
                log("Missing required config values or incorrect type");
                drawErrorMessage(d, "Error: Missing required config values or incorrect type");
                stopProgram(d);
            }
            strlcpy(config.server, doc["server"], sizeof(config.server));
            strlcpy(config.ssid, doc["ssid"], sizeof(config.ssid));
            strlcpy(config.password, doc["password"], sizeof(config.password));
            // strlcpy(config.timezone, doc["timezone"] | defaultConfig.timezone, sizeof(config.timezone));
            config.sleepTime = doc["sleepTime"] | defaultConfig.sleepTime;
            config.wifiTimeout = doc["wifiTimeout"] | defaultConfig.wifiTimeout;
            config.debug = doc["debug"] | defaultConfig.debug;
        }

        // TODO should dump all of the config data
        log("Config data: ");
        Serial.printf("server: %s\nssid: %s\npassword: %s\n", config.server, config.ssid, config.password);
    }
    else
    {
        log("SD Card error!");
        drawErrorMessage(d, "Error: SD Card error");
        stopProgram(d);
    }
}

void saveConfiguration(const char *filename, Config &config)
{
    SdFile file;
    // Open file for writing
    if (!file.open(filename, O_RDWR | O_CREAT | O_TRUNC))
    {
        log("Failed to create file");
        return;
    }

    // Allocate a temporary JsonDocument
    JsonDocument doc;

    log("Setting values in document");
    log("server: " + String(config.server));
    log("ssid: " + String(config.ssid));
    log("password: " + String(config.password));
    log("wifiTimeout: " + String(config.wifiTimeout));
    log("sleepTime: " + String(config.sleepTime));
    log("debug: " + String(config.debug));

    doc["server"] = config.server;
    doc["ssid"] = config.ssid;
    doc["password"] = config.password;
    doc["wifiTimeout"] = config.wifiTimeout;
    doc["sleepTime"] = config.sleepTime;
    doc["debug"] = config.debug;

    // Serialize JSON to file
    if (serializeJsonPretty(doc, file) == 0)
    {
        log("Failed to write to file");
    }

    // Close the file
    file.close();
}

bool hasDisplayedIntroMessage = false;
void readSerialCommands(Config &config)
{
    if (!hasDisplayedIntroMessage)
    {
        log("Serial commands available are debug, ssid, password, server, wifiTimeout, sleepTime, timezone, save, exit, print, reset, help");
        hasDisplayedIntroMessage = true;
    }

    if (Serial.available())
    {
        String command = Serial.readStringUntil('\n');
        if (command == "debug")
        {
            config.debug = !config.debug;
            log("Debug mode: " + String(config.debug));
        }
        else if (command == "ssid")
        {
            String ssid = readUserInput("Enter new SSID:", 10000);
            if (!ssid.isEmpty())
            {
                strlcpy(config.ssid, ssid.c_str(), sizeof(config.ssid));
                log("SSID set to: " + ssid);
            }
        }
        else if (command == "password")
        {
            String password = readUserInput("Enter new password:", 10000);
            if (!password.isEmpty())
            {
                strlcpy(config.password, password.c_str(), sizeof(config.password));
                log("Password set to: " + password);
            }
        }
        else if (command == "server")
        {
            String server = readUserInput("Enter new server:", 10000);
            if (!server.isEmpty())
            {
                strlcpy(config.server, server.c_str(), sizeof(config.server));
                log("Server set to: " + server);
            }
        }
        else if (command == "wifiTimeout")
        {
            String wifiTimeoutStr = readUserInput("Enter new wifiTimeout:", 10000);
            if (!wifiTimeoutStr.isEmpty())
            {
                int wifiTimeout = wifiTimeoutStr.toInt();
                config.wifiTimeout = wifiTimeout;
                log("Wifi timeout set to: " + String(wifiTimeout));
            }
        }
        else if (command == "sleepTime")
        {
            String sleepTimeStr = readUserInput("Enter new sleepTime:", 10000);
            if (!sleepTimeStr.isEmpty())
            {
                int sleepTime = sleepTimeStr.toInt();
                config.sleepTime = sleepTime;
                log("sleepTime set to: " + String(sleepTime));
            }
        }
        else if (command == "save")
        {
            saveConfiguration("/config.txt", config);
            log("Configuration saved");
        }
        else if (command == "current")
        {
            log("Current configuration:");
            log("server: " + String(config.server));
            log("ssid: " + String(config.ssid));
            log("password: " + String(config.password));
            log("wifiTimeout: " + String(config.wifiTimeout));
            log("sleepTime: " + String(config.sleepTime));
            log("debug: " + String(config.debug));
        }
        else if (command == "print")
        {
            log("Printing the contents of the config file, does not reflect current configuration until saved");
            printFile("/config.txt");
        }
        else if (command == "reset")
        {
            config = defaultConfig;
            log("Configuration reset to default");
        }
        else if (command == "restart")
        {
            log("Restarting...");
            Serial.flush();
            ESP.restart();
        }
        else if (command == "help")
        {
            log("Serial commands available are debug, ssid, password, server, wifiTimeout, sleepTime, timezone, save, exit, print, reset, help");
        }
        else
        {
            log("Unknown command");
        }
    }
}

void printFile(const char *filename)
{
    SdFile file;
    // Open file for reading
    if (!file.open(filename, O_RDONLY))
    {
        log("Failed to read file");
        return;
    }

    char buffer[MAX_CONFIG_SIZE];
    int bytesRead;

    // Read and print file content
    while ((bytesRead = file.read(buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytesRead] = '\0'; // Null-terminate the buffer
        Serial.println(buffer);
    }

    file.close();
    log("File read successfully");
}

String readUserInput(const char *prompt, unsigned long timeout)
{
    Serial.println(prompt);
    unsigned long startTime = millis();
    while (!Serial.available() && millis() - startTime < timeout)
    {
        // Non-blocking wait for user input with a timeout
    }
    if (Serial.available())
    {
        return Serial.readStringUntil('\n');
    }
    else
    {
        log(String(prompt) + " input timed out");
        return "";
    }
}