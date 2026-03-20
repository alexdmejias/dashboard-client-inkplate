# What is this
Dashboard project meant to work with the inkplate family of displays. It will ping a server every _n_ seconds and will display the output along with some configuration information at the bottom. Meant to work with something like [dashboard-server](https://github.com/alexdmejias/dashboard-server), but this is meant to be server agnostic, as long as the response is an image, it _should_ work.

## 🚀 Quick Start

**New to this project?** Check out the [Quick Start Guide](QUICKSTART.md) to get up and running in minutes!

**Or flash right now:** Visit the [Web Flasher](https://alexdmejias.github.io/dashboard-client-inkplate/) to flash your device from your browser.

## Features
- microSD configurable
- deep sleep
- external button wake-up
- OTA (Over-The-Air) firmware updates
- ESP Web Tools web-based flashing
- purposefully dumb

## Flashing Firmware

### Option 1: ESP Web Tools (Easiest)
Flash firmware directly from your web browser (Chrome, Edge, or Opera):

**Hosted Flasher (Recommended):**
Visit [https://alexdmejias.github.io/dashboard-client-inkplate/](https://alexdmejias.github.io/dashboard-client-inkplate/) to flash your device with the latest firmware.

**Local Flasher:**
1. Open `flash.html` in a compatible browser
2. Click "Connect and Install"
3. Select your Inkplate device from the serial port list
4. Wait for the flashing process to complete

📖 **[ESP Web Tools Guide](docs/ESP_WEB_TOOLS_GUIDE.md)**

### Option 2: PlatformIO (Serial)
Traditional serial flashing via USB:

```bash
# Install PlatformIO
pip install platformio

# Flash via USB
platformio run --target upload
```

📖 **[Building Firmware Guide](docs/BUILDING_FIRMWARE.md)**

### Option 3: OTA Updates (Wireless)
After initial setup, you can update firmware wirelessly:

1. Enable OTA in your `config.txt`:
   ```json
   {
     "otaEnabled": true,
     "otaPort": 3232,
     "otaPassword": "your-secure-password",
     "otaTimeout": 30
   }
   ```

2. Upload via network:
   ```bash
   platformio run --target upload --upload-port <DEVICE_IP_ADDRESS>
   ```

**Note:** Since the device is in deep sleep most of the time, OTA updates work best when:
- The device is in debug mode (send `debug` via serial during the debug window)
- You trigger the update during the OTA timeout window after wake
- You use the external wake button to wake the device when updates are needed

📖 **[OTA Updates Guide](docs/OTA_GUIDE.md)**

## Documentation

- 📘 [Building Firmware Guide](docs/BUILDING_FIRMWARE.md) - How to build firmware.bin locally or via CI/CD
- 📗 [GitHub Pages Setup](docs/GITHUB_PAGES_SETUP.md) - Host the web flasher on GitHub Pages
- 📕 [OTA Updates Guide](docs/OTA_GUIDE.md) - Wireless firmware updates with deep sleep
- 📙 [ESP Web Tools Guide](docs/ESP_WEB_TOOLS_GUIDE.md) - Web-based flashing instructions

## Roadmap
- show help screen?
- define sleep hours
- run time configurable?
- debug mode to show/hide configuration options
- wifimanager
- decrease amount of refreshes if errors are encountered?

## Configuration
configuration comes from a `config.json` file placed at the root of a MicroSD card. As of this writting, the following options are available:

### hostname(string)
Image server address
### ssid(string)
Wifi SSID to connect to
### password(string)
Plain text password for the wifi network
### sleepTime(int)
Amount of seconds that the screen should sleep for between refreshes
### debugWindow(int)
Seconds to wait after wake for a Serial `debug` command before proceeding. Set to `0` to disable the window.
### showDebug(bool)
Whether to display the configuration overlay on screen
### wifiTimeout(int)
Amount of seconds that should be waited before giving up connecting to WIFI
### wakeButtonPin(int)
GPIO pin number for external button to wake device from deep sleep (default: 36). Connect a button between this pin and GND to wake the device.
### otaEnabled(bool)
Enable Over-The-Air (OTA) firmware updates. When enabled, the device will wait for OTA updates after each wake.
### otaPort(int)
Port number for OTA updates (default: 3232).
### otaPassword(string)
Optional password for OTA updates (for security).
### otaTimeout(int)
Seconds to wait for OTA updates after wake before proceeding with normal operation (default: 30).

## Example Configuration

An example `config.json` file is provided in `example-config.json`. Copy this to the root of your microSD card as `config.txt` and modify the values as needed.

## External Button Wake-up

To use the external button wake-up feature:
1. Configure the `wakeButtonPin` in your config file (default is GPIO 36)
2. Connect a momentary push button between the configured GPIO pin and GND
3. Press the button to wake the device from deep sleep
4. Supported GPIO pins for external wake-up (EXT0): 0, 2, 4, 12-15, 25-27, 32-39

**Note:** The wake-up is triggered when the button is pressed (pin goes LOW). Connect the button between the GPIO pin and GND. The ESP32 will automatically enable an internal pull-up resistor on the pin during deep sleep.

## OTA Updates and Deep Sleep

One of the challenges with OTA updates on devices that spend most of their time in deep sleep is that the device needs to be awake to receive firmware updates. This firmware provides several strategies:

### Strategy 1: Debug Mode
Enter debug mode to keep the device awake:
1. During the `debugWindow` period after wake, send the `debug` command via serial
2. The device will stay awake and continue handling OTA requests
3. Touch any pad (Inkplate 10 only) to exit debug mode and refresh

### Strategy 2: OTA Timeout Window
When `otaEnabled` is `true`, the device waits for the configured `otaTimeout` period after each wake:
1. Set `otaTimeout` to a reasonable duration (e.g., 30-60 seconds)
2. The device will listen for OTA updates during this window after each wake
3. Time your upload to coincide with a scheduled wake cycle

### Strategy 3: External Button Wake
Use the external wake button to wake the device when you need to perform an update:
1. Press the wake button to wake the device
2. The device will enter the OTA timeout window
3. Perform the OTA update during this window

### Finding Your Device's IP Address
To perform OTA updates, you need the device's IP address:
1. Connect via serial and check the logs during WiFi connection
2. Check your router's DHCP client list for a device named `inkplate-XXXXXXXX`
3. The hostname is based on the device's MAC address for uniqueness 