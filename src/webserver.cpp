#include "webserver.h"
#include "config.h"
#include <WiFi.h>
#include <ArduinoJson.h>

extern Config config;

const char HTML_TEMPLATE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Inkplate Configuration</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 600px;
            margin: 20px auto;
            padding: 20px;
            background-color: #f5f5f5;
        }
        .container {
            background-color: white;
            padding: 30px;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        h1 {
            color: #333;
            margin-bottom: 30px;
            text-align: center;
        }
        .form-group {
            margin-bottom: 20px;
        }
        label {
            display: block;
            margin-bottom: 5px;
            color: #555;
            font-weight: bold;
        }
        input[type="text"],
        input[type="password"],
        input[type="number"] {
            width: 100%;
            padding: 10px;
            border: 1px solid #ddd;
            border-radius: 4px;
            box-sizing: border-box;
            font-size: 14px;
        }
        input[type="checkbox"] {
            width: 20px;
            height: 20px;
            margin-right: 10px;
            vertical-align: middle;
        }
        .checkbox-label {
            display: inline;
            font-weight: normal;
        }
        button {
            padding: 12px 30px;
            margin: 10px 5px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-size: 16px;
            font-weight: bold;
        }
        .btn-save {
            background-color: #4CAF50;
            color: white;
        }
        .btn-save:hover {
            background-color: #45a049;
        }
        .btn-restart {
            background-color: #ff9800;
            color: white;
        }
        .btn-restart:hover {
            background-color: #e68900;
        }
        .button-group {
            text-align: center;
            margin-top: 30px;
        }
        .message {
            padding: 15px;
            margin-bottom: 20px;
            border-radius: 4px;
            display: none;
        }
        .success {
            background-color: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        .error {
            background-color: #f8d7da;
            color: #721c24;
            border: 1px solid #f5c6cb;
        }
        @media (max-width: 600px) {
            body {
                margin: 10px;
                padding: 10px;
            }
            .container {
                padding: 15px;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Inkplate Configuration</h1>
        <div id="message" class="message"></div>
        <form id="configForm">
            <div class="form-group">
                <label for="server">Server Hostname:</label>
                <input type="text" id="server" name="server" value="%SERVER%" required>
            </div>
            <div class="form-group">
                <label for="ssid">WiFi SSID:</label>
                <input type="text" id="ssid" name="ssid" value="%SSID%" required>
            </div>
            <div class="form-group">
                <label for="password">WiFi Password:</label>
                <input type="password" id="password" name="password" placeholder="Enter new password to change">
            </div>
            <div class="form-group">
                <label for="wifiTimeout">WiFi Timeout (seconds):</label>
                <input type="number" id="wifiTimeout" name="wifiTimeout" value="%WIFI_TIMEOUT%" min="1" max="300" required>
            </div>
            <div class="form-group">
                <label for="sleepTime">Sleep Time (seconds):</label>
                <input type="number" id="sleepTime" name="sleepTime" value="%SLEEP_TIME%" min="1" max="86400" required>
            </div>
            <div class="form-group">
                <label for="debugWindow">Debug Window (seconds):</label>
                <input type="number" id="debugWindow" name="debugWindow" value="%DEBUG_WINDOW%" min="0" max="600" required>
            </div>
            <div class="form-group">
                <label for="wakeButtonPin">Wake Button Pin (GPIO):</label>
                <input type="number" id="wakeButtonPin" name="wakeButtonPin" value="%WAKE_BUTTON_PIN%" min="0" max="39" required>
            </div>
            <div class="form-group">
                <input type="checkbox" id="showDebug" name="showDebug" %SHOW_DEBUG_CHECKED%>
                <label for="showDebug" class="checkbox-label">Show Debug Overlay</label>
            </div>
            <div class="button-group">
                <button type="submit" class="btn-save">Save Configuration</button>
                <button type="button" class="btn-restart" onclick="restartDevice()">Restart Device</button>
            </div>
        </form>
    </div>
    <script>
        function showMessage(text, isSuccess) {
            const msgDiv = document.getElementById('message');
            msgDiv.textContent = text;
            msgDiv.className = 'message ' + (isSuccess ? 'success' : 'error');
            msgDiv.style.display = 'block';
            setTimeout(() => {
                msgDiv.style.display = 'none';
            }, 5000);
        }

        document.getElementById('configForm').addEventListener('submit', async (e) => {
            e.preventDefault();
            const formData = new FormData(e.target);
            const data = {};
            formData.forEach((value, key) => {
                if (key === 'showDebug') {
                    data[key] = true;
                } else {
                    data[key] = value;
                }
            });
            if (!formData.has('showDebug')) {
                data.showDebug = false;
            }

            try {
                const response = await fetch('/save', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                    },
                    body: JSON.stringify(data)
                });
                
                if (response.ok) {
                    showMessage('Configuration saved successfully! Please restart the device.', true);
                } else {
                    showMessage('Failed to save configuration.', false);
                }
            } catch (error) {
                showMessage('Error: ' + error.message, false);
            }
        });

        function restartDevice() {
            if (confirm('Are you sure you want to restart the device?')) {
                fetch('/restart', {method: 'POST'})
                    .then(() => {
                        showMessage('Device is restarting...', true);
                        setTimeout(() => {
                            window.location.href = '/';
                        }, 3000);
                    })
                    .catch(error => {
                        showMessage('Error: ' + error.message, false);
                    });
            }
        }
    </script>
</body>
</html>
)rawliteral";

String generateHTML(Config &config) {
    String html = String(HTML_TEMPLATE);
    html.replace("%SERVER%", String(config.server));
    html.replace("%SSID%", String(config.ssid));
    html.replace("%WIFI_TIMEOUT%", String(config.wifiTimeout));
    html.replace("%SLEEP_TIME%", String(config.sleepTime));
    html.replace("%DEBUG_WINDOW%", String(config.debugWindow));
    html.replace("%WAKE_BUTTON_PIN%", String(config.wakeButtonPin));
    html.replace("%SHOW_DEBUG_CHECKED%", config.showDebug ? "checked" : "");
    return html;
}

void setupWebServer(AsyncWebServer &server, Config &config) {
    log("Setting up web server...");

    // Serve the main configuration page
    server.on("/", HTTP_GET, [&config](AsyncWebServerRequest *request) {
        log("Serving configuration page");
        String html = generateHTML(config);
        request->send(200, "text/html", html);
    });

    // Handle configuration save
    server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
        [&config](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
            log("Received save request");
            
            String body = "";
            for (size_t i = 0; i < len; i++) {
                body += (char)data[i];
            }
            
            log("Request body: " + body);
            
            // Parse JSON
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, body);
            
            if (error) {
                log("Failed to parse JSON: " + String(error.c_str()));
                request->send(400, "text/plain", "Invalid JSON");
                return;
            }
            
            // Validate and update configuration
            if (doc.containsKey("server") && doc["server"].as<String>().length() > 0) {
                strlcpy(config.server, doc["server"].as<const char*>(), sizeof(config.server));
            }
            if (doc.containsKey("ssid") && doc["ssid"].as<String>().length() > 0) {
                strlcpy(config.ssid, doc["ssid"].as<const char*>(), sizeof(config.ssid));
            }
            // Only update password if it's not empty (allows keeping existing password)
            if (doc.containsKey("password") && doc["password"].as<String>().length() > 0) {
                strlcpy(config.password, doc["password"].as<const char*>(), sizeof(config.password));
            }
            if (doc.containsKey("wifiTimeout")) {
                int value = doc["wifiTimeout"].as<int>();
                if (value >= 1 && value <= 300) {  // 1-300 seconds
                    config.wifiTimeout = value;
                } else {
                    log("Invalid wifiTimeout value: " + String(value));
                    request->send(400, "text/plain", "Invalid wifiTimeout (must be 1-300)");
                    return;
                }
            }
            if (doc.containsKey("sleepTime")) {
                int value = doc["sleepTime"].as<int>();
                if (value >= 1 && value <= 86400) {  // 1-86400 seconds (1 day)
                    config.sleepTime = value;
                } else {
                    log("Invalid sleepTime value: " + String(value));
                    request->send(400, "text/plain", "Invalid sleepTime (must be 1-86400)");
                    return;
                }
            }
            if (doc.containsKey("debugWindow")) {
                int value = doc["debugWindow"].as<int>();
                if (value >= 0 && value <= 600) {  // 0-600 seconds
                    config.debugWindow = value;
                } else {
                    log("Invalid debugWindow value: " + String(value));
                    request->send(400, "text/plain", "Invalid debugWindow (must be 0-600)");
                    return;
                }
            }
            if (doc.containsKey("wakeButtonPin")) {
                int value = doc["wakeButtonPin"].as<int>();
                // Note: Not all ESP32 GPIO pins are usable. Pins 6-11 are connected to flash.
                // Commonly used wake pins include: 0, 2, 4, 12-15, 25-27, 32-39
                if (value >= 0 && value <= 39) {  // Valid ESP32 GPIO pins
                    config.wakeButtonPin = value;
                } else {
                    log("Invalid wakeButtonPin value: " + String(value));
                    request->send(400, "text/plain", "Invalid wakeButtonPin (must be 0-39)");
                    return;
                }
            }
            if (doc.containsKey("showDebug")) {
                config.showDebug = doc["showDebug"].as<bool>();
            }
            
            // Save to SD card
            saveConfiguration("/config.txt", config);
            
            log("Configuration saved successfully");
            request->send(200, "text/plain", "Configuration saved");
        }
    );

    // Handle device restart
    server.on("/restart", HTTP_POST, [](AsyncWebServerRequest *request) {
        log("Restart requested");
        request->send(200, "text/plain", "Restarting...");
        delay(500);
        ESP.restart();
    });

    // Handle 404
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not found");
    });

    server.begin();
    log("Web server started");
}

bool connectToWifiForDebug(Inkplate &d, const char *ssid, const char *password, int timeout, String &ipAddress, String &connectionMode) {
    log("Attempting to connect to WiFi...");
    
    // Try to connect to configured WiFi
    bool connected = d.connectWiFi(ssid, password, timeout, true);
    
    if (connected) {
        ipAddress = WiFi.localIP().toString();
        connectionMode = "WiFi";
        log("Connected to WiFi: " + String(ssid));
        log("IP Address: " + ipAddress);
        return true;
    }
    
    // If WiFi connection fails, start Access Point
    log("WiFi connection failed, starting Access Point...");
    WiFi.mode(WIFI_AP);
    const char* apPassword = "inkplate123";  // Default password for AP
    bool apStarted = WiFi.softAP("Inkplate-Config", apPassword);
    
    if (apStarted) {
        ipAddress = WiFi.softAPIP().toString();
        connectionMode = "AP";
        log("Access Point started");
        log("SSID: Inkplate-Config");
        log("Password: " + String(apPassword));
        log("IP Address: " + ipAddress);
        return true;
    }
    
    log("Failed to start Access Point");
    return false;
}
