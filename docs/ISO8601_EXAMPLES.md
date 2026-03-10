# ISO 8601 Duration Format Examples

This document provides examples of using ISO 8601 duration format with the dashboard-client-inkplate sleep interval configuration.

## Basic Format

ISO 8601 duration format: `P[n]DT[n]H[n]M[n]S`

- `P` = Period designator (required, must be first)
- `[n]D` = Number of days
- `T` = Time designator (separates date from time components)
- `[n]H` = Number of hours (after T)
- `[n]M` = Number of minutes (after T)
- `[n]S` = Number of seconds (after T)

## Examples for Common Use Cases

### Short Intervals (Seconds/Minutes)

```javascript
// JavaScript Temporal API usage
const duration = Temporal.Duration.from({ seconds: 30 });
duration.toString(); // "PT30S"

// HTTP Header
x-sleep-for: PT30S
```

```javascript
// 5 minutes
const duration = Temporal.Duration.from({ minutes: 5 });
duration.toString(); // "PT5M"

// HTTP Header
x-sleep-for: PT5M
```

### Medium Intervals (Hours)

```javascript
// 2 hours
const duration = Temporal.Duration.from({ hours: 2 });
duration.toString(); // "PT2H"

// HTTP Header
x-sleep-for: PT2H
```

```javascript
// 1 hour 30 minutes
const duration = Temporal.Duration.from({ hours: 1, minutes: 30 });
duration.toString(); // "PT1H30M"

// HTTP Header
x-sleep-for: PT1H30M
```

### Long Intervals (Days)

```javascript
// 1 day
const duration = Temporal.Duration.from({ days: 1 });
duration.toString(); // "P1D"

// HTTP Header
x-sleep-for: P1D
```

```javascript
// 1 day and 12 hours
const duration = Temporal.Duration.from({ days: 1, hours: 12 });
duration.toString(); // "P1DT12H"

// HTTP Header
x-sleep-for: P1DT12H
```

### Complex Durations

```javascript
// 2 hours, 30 minutes, 45 seconds
const duration = Temporal.Duration.from({ 
  hours: 2, 
  minutes: 30, 
  seconds: 45 
});
duration.toString(); // "PT2H30M45S"

// HTTP Header
x-sleep-for: PT2H30M45S
```

```javascript
// 1 day, 2 hours, 30 minutes, 45 seconds
const duration = Temporal.Duration.from({ 
  days: 1,
  hours: 2, 
  minutes: 30, 
  seconds: 45 
});
duration.toString(); // "P1DT2H30M45S"

// HTTP Header
x-sleep-for: P1DT2H30M45S
```

## Server Implementation Example (Node.js with Temporal)

```javascript
const express = require('express');
const { Temporal } = require('@js-temporal/polyfill');

const app = express();

app.get('/image', (req, res) => {
  // Determine sleep interval based on time of day or other logic
  const hour = new Date().getHours();
  
  let sleepDuration;
  if (hour >= 0 && hour < 6) {
    // Night time: update every 8 hours
    sleepDuration = Temporal.Duration.from({ hours: 8 });
  } else if (hour >= 6 && hour < 18) {
    // Day time: update every 15 minutes
    sleepDuration = Temporal.Duration.from({ minutes: 15 });
  } else {
    // Evening: update every 30 minutes
    sleepDuration = Temporal.Duration.from({ minutes: 30 });
  }
  
  // Set the ISO 8601 duration header
  res.set('x-sleep-for', sleepDuration.toString());
  
  // Send the image
  res.sendFile('/path/to/image.bmp');
});

app.listen(3000);
```

## Comparison with Simple Format

| Simple Format | ISO 8601 Format | Seconds |
|--------------|-----------------|---------|
| `30s` | `PT30S` | 30 |
| `5m` | `PT5M` | 300 |
| `2h` | `PT2H` | 7200 |
| `1d` | `P1D` | 86400 |
| N/A | `PT1H30M` | 5400 |
| N/A | `P1DT2H30M45S` | 95445 |

## Notes

- ISO 8601 format is case-insensitive (`pt30s` works the same as `PT30S`)
- All components are optional, but at least one must be present
- Components must be in order: Days before T, then Hours, Minutes, Seconds
- Zero values are not allowed (e.g., `PT0S` will fail)
- The format is compatible with JavaScript Temporal API, making server-side code cleaner
- Years and months are not supported (they're ambiguous in duration calculations)
