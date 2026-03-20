# What is this
Dashboard project meant to work with the inkplate family of displays. It will ping a server every _n_ seconds and will display the output along with some configuration information at the bottom. Meant to work with something like [dashboard-server](https://github.com/alexdmejias/dashboard-server), but this is meant to be server agnostic, as long as the response is an image, it _should_ work.

## Features
- microSD configurable
- deep sleep
- external button wake-up
- optimized power consumption (Bluetooth disabled, WiFi shutdown, reduced CPU frequency)
- purposefully dumb

## Battery Saving

This firmware is optimized for battery-powered operation with several power-saving features:

- **Deep Sleep Mode**: Ultra-low power consumption between refreshes (<150 µA)
- **Explicit WiFi Shutdown**: WiFi radio disabled after use to prevent residual power drain
- **Bluetooth Disabled**: Bluetooth stack disabled at startup (not used)
- **Reduced CPU Frequency**: CPU runs at 80 MHz instead of 240 MHz during operation
- **SD Card Sleep**: SD card powered down during deep sleep
- **Configurable Timeouts**: WiFi and HTTP connection timeouts prevent prolonged operations

For detailed information about battery life optimization, power consumption estimates, and best practices, see [docs/BATTERY_SAVING.md](docs/BATTERY_SAVING.md).

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

## Example Configuration

An example `config.json` file is provided in `example-config.json`. Copy this to the root of your microSD card as `config.txt` and modify the values as needed.

## External Button Wake-up

To use the external button wake-up feature:
1. Configure the `wakeButtonPin` in your config file (default is GPIO 36)
2. Connect a momentary push button between the configured GPIO pin and GND
3. Press the button to wake the device from deep sleep
4. Supported GPIO pins for external wake-up (EXT0): 0, 2, 4, 12-15, 25-27, 32-39

**Note:** The wake-up is triggered when the button is pressed (pin goes LOW). Connect the button between the GPIO pin and GND. The ESP32 will automatically enable an internal pull-up resistor on the pin during deep sleep. 