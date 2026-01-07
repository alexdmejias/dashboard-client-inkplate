# What is this
Dashboard project meant to work with the inkplate family of displays. It will ping a server every _n_ seconds and will display the output along with some configuration information at the bottom. Meant to work with something like [dashboard-server](https://github.com/alexdmejias/dashboard-server), but this is meant to be server agnostic, as long as the response is an image, it _should_ work.

## Features
- microSD configurable
- deep sleep
- external button wake-up
- debug mode with web interface for configuration
- purposefully dumb

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

## Debug Mode

The device supports a debug mode that can be activated in two ways:
1. **Touchpad Wake**: Wake the device using the touchpad (on supported models)
2. **Serial Command**: Send the `debug` command via Serial during the debug window

When in debug mode, the device will:
- Connect to the configured WiFi network, or create an Access Point named "Inkplate-Config" if connection fails
- Start a web server on port 80
- Display the connection information on the e-ink screen
- Provide a web interface for configuration

### Web Configuration Interface

The web interface can be accessed by navigating to the IP address displayed on the screen (e.g., `http://192.168.1.100`). The interface provides:

- Input fields for all configuration parameters:
  - Server hostname
  - WiFi SSID and password
  - WiFi timeout (seconds)
  - Sleep time (seconds)
  - Debug window (seconds)
  - Wake button GPIO pin
  - Show debug overlay (checkbox)
- **Save Configuration** button to save settings to SD card
- **Restart Device** button to restart the device

The web interface works on both desktop and mobile devices and displays current configuration values pre-populated in the form.

### Exiting Debug Mode

To exit debug mode:
- Touch any touchpad on the device (on supported models), or
- Press the **Restart Device** button in the web interface, or
- Use the `restart` command via Serial

The serial command interface remains available during debug mode for those who prefer terminal-based configuration. 