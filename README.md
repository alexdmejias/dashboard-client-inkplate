# What is this
Dashboard project meant to work with the inkplate family of displays. It will ping a server every _n_ seconds and will display the output along with some configuration information at the bottom. Meant to work with something like [dashboard-server](https://github.com/alexdmejias/dashboard-server), but this is meant to be server agnostic, as long as the response is an image, it _should_ work.

## Features
- microSD configurable
- deep sleep
- purposefully dumb
- **Dynamic sleep intervals via HTTP headers** - Server can control refresh rates

## Roadmap
- show help screen?
- ~~define sleep hours~~ ✅ **Implemented via HTTP headers**
- ~~run time configurable?~~ ✅ **Implemented via HTTP headers**
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
Amount of seconds that the screen should sleep for between refreshes. **Note:** This can be overridden dynamically by HTTP response headers (see Dynamic Sleep Intervals below).
### debug(bool)
Whether to display configuration being used
### wifiTimeout(int)
Amount of seconds that should be waited before giving up connecting to WIFI
### timezoneOffset(int)
**Optional.** Timezone offset in hours from UTC (e.g., `-5` for EST, `+1` for CET, `+5.5` for IST). When specified, schedule-based sleep intervals will be interpreted in local time instead of UTC. Default is `0` (UTC).

## Dynamic Sleep Intervals

The device supports dynamic sleep interval configuration via HTTP response headers from the image server. This allows the server to control refresh rates without requiring device reconfiguration.

### Header Support
The device checks for sleep interval configuration in the following HTTP response headers (in order of preference):
1. `x-sleep-for`
2. `refresh-interval`

### Supported Formats

#### Simple Time Formats
Use a number followed by a time unit:
- `s` - seconds (e.g., `30s` = 30 seconds)
- `m` - minutes (e.g., `5m` = 5 minutes)
- `h` - hours (e.g., `2h` = 2 hours)
- `d` - days (e.g., `1d` = 1 day)

**Example:**
```http
x-sleep-for: 5m
```

#### Schedule Format
Define different intervals for different times of day using a space-separated list of time ranges:

**Format:** `STARTHOUR:STARTMINUTE-ENDHOUR:ENDMINUTE=INTERVAL`

Where:
- `STARTHOUR:STARTMINUTE` - Time this setting starts (24-hour format)
- `ENDHOUR:ENDMINUTE` - Time this setting ends (24-hour format)
- `INTERVAL` - Update interval in **MINUTES**

**Example:**
```http
x-sleep-for: 00:00-06:00=480 06:00-18:00=15 18:00-24:00=30
```

This schedules:
- Midnight to 6am: Update every 480 minutes (8 hours)
- 6am to 6pm: Update every 15 minutes
- 6pm to midnight: Update every 30 minutes

### Timezone Support

By default, schedule times are interpreted as UTC. To use your local timezone, set the `timezoneOffset` parameter in your config.json file:

```json
{
  "timezoneOffset": -5
}
```

Common timezone offsets:
- **EST/EDT**: `-5` (Eastern US)
- **PST/PDT**: `-8` (Pacific US)
- **CET/CEST**: `+1` (Central Europe)
- **IST**: `+5.5` (India)
- **JST**: `+9` (Japan)

With `timezoneOffset: -5`, a schedule of `08:00-17:00=15` will match 8am-5pm EST, not UTC.

**Note:** Time ranges must not wrap over midnight.

### Fallback Behavior
The device uses the following priority order for sleep intervals:
1. HTTP response header (if present and valid)
2. `sleepTime` from config.json
3. 3000 seconds (default)

### Requirements
- **NTP Time Sync**: Schedule-based intervals require successful NTP time synchronization
- If time sync fails, schedule formats will fall back to config file settings
- Simple time formats work without NTP sync 