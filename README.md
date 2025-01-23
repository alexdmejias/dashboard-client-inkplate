# What is this
Dashboard project meant to work with the inkplate family of displays. It will ping a server every _n_ seconds and will display the output along with some configuration information at the bottom. Meant to work with something like [dashboard-server](https://github.com/alexdmejias/dashboard-server), but this is meant to be server agnostic, as long as the response is an image, it _should_ work.

## Features
- microSD configurable
- deep sleep
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
Amount of seconds that the screen should sleep for
### debug(bool)
Whether to display configuration being used