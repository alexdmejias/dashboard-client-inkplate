# Sleep Status Display Feature

## Overview

When `showSleepStatus` is enabled in the configuration, the Inkplate device will display sleep information before entering deep sleep mode. This provides visual feedback about when the device went to sleep and when it will wake up.

## Configuration

Add to your `config.json` file:

```json
{
  "showSleepStatus": true,
  "timezoneOffset": -5
}
```

Or toggle via serial command:
```
showSleepStatus
```

## Display Layout

The sleep status screen shows the following information in a centered layout:

```
┌─────────────────────────────────────────────┐
│                                             │
│          Going to Sleep                     │  (24pt, centered)
│     2026-03-10 14:30:00 UTC-5              │
│            for 2h 30m                       │
│     Wake: 2026-03-10 17:00:00              │
│                                             │
└─────────────────────────────────────────────┘
```

**Note**: The display uses the existing `drawErrorMessage()` function for consistent styling.

## Duration Formatting

The duration is displayed in the most appropriate human-readable format:

| Sleep Duration | Display Format | Example |
|---------------|----------------|---------|
| < 60 seconds | Xs | 30s |
| 60s - 1h | Xm Ys | 5m 30s |
| 1h - 24h | Xh Ym | 2h 30m |
| ≥ 24h | Xd Yh Zm | 1d 3h 15m |

## Timezone Support

When `timezoneOffset` is configured, all times are displayed in the local timezone:

- **Without offset** (UTC): `2026-03-10 14:30:00 UTC`
- **With offset** (EST, -5): `2026-03-10 09:30:00 UTC-5`
- **With offset** (CET, +1): `2026-03-10 15:30:00 UTC+1`

## Use Cases

### Development & Testing
Enable `showSleepStatus` during development to:
- Verify sleep intervals are correct
- Debug schedule-based sleep behavior
- Monitor device sleep/wake cycles
- Confirm timezone offset calculations

### Production Monitoring
Enable in production for:
- Visual confirmation of device status
- Troubleshooting sleep-related issues
- Understanding device behavior in the field
- Providing user feedback in kiosk installations

### Disable for Normal Operation
Disable `showSleepStatus` for:
- Standard deployments where the image should remain visible
- Battery optimization (skip extra display refresh)
- When sleep status is not needed by end users

## Manual Wake-Up

The sleep status display includes the message "Touch any pad to wake early", indicating that users can manually wake the device by touching any of the three integrated touchpads on the Inkplate10/10v2.

This feature works whether or not `showSleepStatus` is enabled - touchpad wake-up is always active for convenience and troubleshooting.

## Example Configuration Files

### Minimal with Sleep Status
```json
{
  "server": "https://example.com/image",
  "ssid": "MyWiFi",
  "password": "mypassword",
  "showSleepStatus": true
}
```

### Full Configuration
```json
{
  "server": "https://example.com/image",
  "ssid": "MyWiFi",
  "password": "mypassword",
  "sleepTime": 3600,
  "timezoneOffset": -5,
  "showSleepStatus": true,
  "debug": false,
  "wifiTimeout": 30
}
```

## Technical Details

- Display uses the existing `drawErrorMessage()` function for consistent styling
- Message formatted inline within `handleSleep()` function in global.cpp
- Time calculations account for timezone offset
- Wake time is computed as current_time + sleep_duration
- Display is refreshed before entering deep sleep
- Touchpad wake-up enabled via `esp_sleep_enable_ext1_wakeup()`
