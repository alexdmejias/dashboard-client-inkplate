# What is this
Dashboard project meant to work with the inkplate family of displays. It will ping a server every _n_ seconds and will display the output along with some configuration information at the bottom. Meant to work with something like [dashboard-server](https://github.com/alexdmejias/dashboard-server), but this is meant to be server agnostic, as long as the response is an image, it _should_ work.

## Features
- microSD configurable
- deep sleep
- external button wake-up
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
### debug(bool)
Whether to display configuration being used
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

**Note:** The wake-up is triggered when the button is pressed (pin goes LOW), so make sure to connect it to GND with no pull-up resistor needed (internal pull-up will be used). 