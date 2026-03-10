# What is this
Dashboard project meant to work with the inkplate family of displays. It will ping a server every _n_ seconds and will display the output along with some configuration information at the bottom. Meant to work with something like [dashboard-server](https://github.com/alexdmejias/dashboard-server), but this is meant to be server agnostic, as long as the response is an image, it _should_ work.

## Features
- microSD configurable
- deep sleep
- external button wake-up
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
### debugWindow(int)
Seconds to wait after wake for a Serial `debug` command before proceeding. Set to `0` to disable the window.
### showDebug(bool)
Whether to display the configuration overlay on screen
### wifiTimeout(int)
Amount of seconds that should be waited before giving up connecting to WIFI
### httpTimeout(int)
Timeout for HTTP requests in seconds (default: 15)
### timezoneOffset(float)
**Optional.** Timezone offset in hours from UTC (e.g., `-5` for EST, `+1` for CET, `+5.5` for IST). When specified, schedule-based sleep intervals will be interpreted in local time instead of UTC. Default is `0` (UTC).
### showSleepStatus(bool)
**Optional.** When enabled, the display will show sleep information before entering deep sleep, including the current time, sleep duration, and expected wake-up time. Default is `false` (disabled).
### wakeButtonPin(int)
GPIO pin number for external button to wake device from deep sleep (default: 36). Connect a button between this pin and GND to wake the device.

## Example Configuration

An example `config.json` file is provided in `example-config.json`. Copy this to the root of your microSD card as `config.txt` and modify the values as needed.

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

#### ISO 8601 Duration Format
Use standard ISO 8601 duration format (compatible with JavaScript Temporal API):

**Format:** `P[n]DT[n]H[n]M[n]S`

Where:
- `P` - Period designator (required, must be first)
- `[n]D` - Number of days
- `T` - Time designator (separates date from time components)
- `[n]H` - Number of hours (after T)
- `[n]M` - Number of minutes (after T)
- `[n]S` - Number of seconds (after T)

**Examples:**
```http
x-sleep-for: PT30S          # 30 seconds
x-sleep-for: PT5M           # 5 minutes
x-sleep-for: PT2H           # 2 hours
x-sleep-for: P1D            # 1 day
x-sleep-for: PT1H30M        # 1 hour 30 minutes
x-sleep-for: P1DT2H30M45S   # 1 day, 2 hours, 30 minutes, 45 seconds
```

**Note:** Components must be in order (D before T, H before M before S). All components are optional, but at least one must be present.

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

### Sleep Status Display

When `showSleepStatus` is enabled in the configuration, the device will display sleep information before entering deep sleep mode:

```json
{
  "showSleepStatus": true
}
```

The display will show:
- **Current time** when going to sleep (with timezone)
- **Sleep duration** in human-readable format (e.g., "2h 30m")
- **Wake-up time** (calculated based on current time + duration)
- **Wake-up instruction** (how to manually wake the device early)

This feature is useful for monitoring device status and understanding sleep schedules.

## External Button Wake-up

To use the external button wake-up feature:
1. Configure the `wakeButtonPin` in your config file (default is GPIO 36)
2. Connect a momentary push button between the configured GPIO pin and GND
3. Press the button to wake the device from deep sleep
4. Supported GPIO pins for external wake-up (EXT0): 0, 2, 4, 12-15, 25-27, 32-39

**Note:** The wake-up is triggered when the button is pressed (pin goes LOW). Connect the button between the GPIO pin and GND. The ESP32 will automatically enable an internal pull-up resistor on the pin during deep sleep.

### Manual Wake-Up Mechanisms

The device supports manual wake-up during sleep through multiple mechanisms:

#### Touchpad Wake-Up (Recommended)
- **Inkplate10/10v2**: Built-in touchpads (PAD1, PAD2, PAD3) can wake the device
- **No hardware changes needed**: Touchpad wake-up is now enabled by default
- **Usage**: Simply touch any of the three touchpad areas on the device
- **Response**: Device will immediately wake and enter debug mode, allowing you to reconfigure or restart

#### External Button Wake-Up (Optional)
- Connect a button to the configured GPIO pin (default: GPIO 36)
- Button press wakes device from deep sleep
- Provides a physical wake-up method in addition to touchpad

#### How It Works
When the device enters deep sleep with touchpad wake-up enabled:
1. Touch any of the three touchpads (PAD1, PAD2, PAD3) or press external button
2. Device wakes up and detects wake source
3. Enters debug mode with configuration options
4. Touch touchpad again or restart to resume normal operation

This provides a convenient way to interrupt sleep cycles for maintenance, reconfiguration, or immediate updates without waiting for the timer to expire.
