# Battery Saving Features in Dashboard Client

This document describes the battery saving features implemented in the Inkplate dashboard client and provides guidance on optimizing battery life.

## Currently Implemented Features

### 1. Deep Sleep Mode
**Primary Power Saving Strategy**
- Uses ESP32's deep sleep mode via `esp_deep_sleep_start()`
- Power consumption in deep sleep: ~10-100 µA (microamps)
- All peripherals except RTC are powered down
- Device wakes only on configured triggers

**Implementation:**
```cpp
esp_deep_sleep_start(); // in src/global.cpp handleSleep()
```

### 2. Configurable Sleep Duration
**Flexible Update Intervals**
- Configure via `sleepTime` parameter in config.json (seconds)
- Server can override via `x-sleep-for` HTTP response header
- Default: 20 seconds (configurable up to hours/days)

**Power Impact:**
- Longer sleep = better battery life
- 5-minute intervals: weeks of battery life
- 1-hour intervals: months of battery life

### 3. Wake-Up Mechanisms

#### Timer-Based Wake-up
- Automatically wakes device after configured sleep duration
- Enables periodic refresh without user interaction

#### External Button Wake-up
- Allows manual refresh by pressing a button
- GPIO pin configurable via `wakeButtonPin` (default: GPIO 36)
- Connect button between GPIO pin and GND
- Valid pins: 0, 2, 4, 12-15, 25-27, 32-39

#### Touchpad Wake-up (Inkplate 10 only)
- Built-in touchpads can wake device
- Enables debug mode when device wakes via touchpad

### 4. SD Card Power Management
**Reduces Quiescent Current**
- SD card put to sleep before deep sleep: `d.sdCardSleep()`
- Saves 1-5 mA when in deep sleep

### 5. Connection Timeouts
**Prevents Battery Drain from Stalled Operations**

#### WiFi Timeout
- Default: 30 seconds
- Configurable via `wifiTimeout` in config.json
- Prevents prolonged WiFi connection attempts

#### HTTP Timeout
- Default: 15 seconds
- Configurable via `httpTimeout` in config.json
- Prevents stalled network downloads

### 6. Optimized Network Operations
**Stream Settings**
```cpp
http.getStream().setNoDelay(true);  // Disable Nagle's algorithm for faster transfers
```

### 7. Debug Mode Control
**Conditional Feature Activation**
- `debugWindow` parameter: seconds to wait for debug command (default: 10)
- Set to 0 to disable and save power
- Debug mode only activates when explicitly requested

## Additional Battery Saving Improvements

The following improvements have been identified and implemented:

### 8. Explicit WiFi Shutdown
**Ensures WiFi Radio is Disabled**
- WiFi disconnected after image fetch: `WiFi.disconnect(true)`
- WiFi mode set to OFF: `WiFi.mode(WIFI_OFF)`
- Saves 70-100 mA during deep sleep if WiFi was left in low-power mode

**Implementation:** Added to `getImage()` function after HTTP operation

### 9. Bluetooth Radio Disabled
**Disables Unused Bluetooth**
- Explicitly disable Bluetooth at startup
- Saves 10-50 mA if Bluetooth was enabled by default

**Implementation:** Added `btStop()` in `setup()` before network operations

### 10. Reduced CPU Frequency During Operation
**Lower Power Consumption During Active Period**
- Reduce CPU from 240 MHz to 80 MHz after initialization
- Saves 30-50 mA during active operation
- Minimal impact on performance (network I/O bound)

**Implementation:** Added `setCpuFrequencyMhz(80)` after display initialization

**Note:** Full 240 MHz speed is available during boot for fast initialization, then reduced for normal operation.

## Power Consumption Estimates

### Active Period (fetching and displaying image)
- Duration: 5-30 seconds (depending on network speed and image size)
- Current: ~150-200 mA (with optimizations)
- Components active: ESP32 CPU, WiFi, Display, SD card

### Deep Sleep Period
- Duration: 20 seconds to hours (configurable)
- Current: ~10-100 µA (microamps)
- Only RTC and wake-up circuits active

### Battery Life Calculation Example

**Scenario:** 3000 mAh battery, 5-minute refresh interval

**Active:** 
- 20 seconds every 5 minutes = 20s / 300s = 6.67% duty cycle
- Active current: 175 mA (average of 150-200 mA range)
- Average active: 175 mA × 0.0667 = 11.7 mA

**Sleep:**
- 280 seconds every 5 minutes = 93.33% duty cycle
- Sleep current: 55 µA = 0.055 mA (average of 10-100 µA range)
- Average sleep: 0.055 mA × 0.9333 = 0.051 mA
- **Note:** Actual sleep current varies with hardware revision and temperature; use conservative estimates for critical applications

**Total Average:** 11.7 + 0.051 = 11.75 mA

**Battery Life:** 3000 mAh / 11.75 mA = **255 hours = 10.6 days**

## Best Practices for Maximum Battery Life

1. **Increase Sleep Interval**
   - Set `sleepTime` to 3600 (1 hour) or more
   - Use server `x-sleep-for` header for dynamic control

2. **Minimize Image Size**
   - Smaller images download faster = less active time
   - Use optimized BMP format (1, 4, 8, or 24-bit)

3. **Optimize Network**
   - Strong WiFi signal reduces connection time
   - Place device near access point

4. **Disable Debug Features**
   - Set `debugWindow` to 0
   - Set `showDebug` to false

5. **Use Lower Refresh Rates**
   - E-ink displays retain image indefinitely
   - Updates every few hours often sufficient

6. **Monitor WiFi Timeouts**
   - Increase `wifiTimeout` if network is slow but reliable
   - Decrease if network often fails to save power

## Measuring Power Consumption

To measure actual power consumption:

1. **Equipment Needed**
   - Multimeter with µA range
   - Inkplate device with battery
   - Test setup with current measurement in battery line

2. **Measurement Points**
   - Deep sleep current (should be <100 µA)
   - Active current during image fetch (150-200 mA)
   - Peak current during display update (200-300 mA)

3. **Validation**
   - If deep sleep current > 1 mA, check for:
     - WiFi not disabled properly
     - Bluetooth still active
     - SD card not in sleep mode
     - External peripherals drawing current

## Configuration Examples

### Maximum Battery Life (1-2 months)
```json
{
  "sleepTime": 3600,
  "wifiTimeout": 30,
  "httpTimeout": 15,
  "debugWindow": 0,
  "showDebug": false
}
```

### Balanced (1-2 weeks)
```json
{
  "sleepTime": 300,
  "wifiTimeout": 30,
  "httpTimeout": 15,
  "debugWindow": 10,
  "showDebug": true
}
```

### Development/Testing (hours)
```json
{
  "sleepTime": 20,
  "wifiTimeout": 30,
  "httpTimeout": 15,
  "debugWindow": 10,
  "showDebug": true
}
```

## Roadmap for Future Improvements

- [ ] Battery voltage monitoring and display
- [ ] Adaptive sleep based on battery level
- [ ] Partial display refresh to reduce update power
- [ ] WiFi connection persistence (avoid reconnecting every wake)
- [ ] Smart scheduling (sleep longer at night, more updates during day)
- [ ] Exponential backoff on repeated errors
- [ ] Ultra-low-power RTC wake-up source

## References

- [ESP32 Power Management](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/power_management.html)
- [ESP32 Deep Sleep](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/sleep_modes.html)
- [Inkplate Documentation](https://inkplate.readthedocs.io/)
