# Flashing Notes

## ESP Web Tools Manifest Configuration

The `manifest.json` file is configured for ESP Web Tools flashing with offset 0. This is correct for merged firmware binaries.

### Why offset 0?

When PlatformIO builds the firmware, it creates a `firmware.bin` that is a merged binary including:
- Bootloader
- Partition table
- Application firmware

This merged binary is designed to be written starting at offset 0 (the beginning of flash). ESP Web Tools will write this entire merged binary, correctly positioning all components.

### Alternative: Multi-part Flashing

If you want to flash individual components (not recommended for beginners), you would need:
```json
{
  "parts": [
    { "path": "bootloader.bin", "offset": 4096 },
    { "path": "partitions.bin", "offset": 32768 },
    { "path": "firmware.bin", "offset": 65536 }
  ]
}
```

However, this requires extracting individual binaries from the build process and is more complex.

## Power Consumption During OTA

The OTA timeout window uses a 250ms delay between ArduinoOTA.handle() calls. This provides:
- Fast enough response for OTA connection detection
- Reduced power consumption vs. continuous polling
- Still maintains WiFi connection during the wait period

For even lower power consumption, consider:
- Setting a shorter otaTimeout (e.g., 15-20 seconds)
- Using debug mode for planned updates (device stays awake but user-controlled)
- Scheduling updates for specific times using the external wake button
