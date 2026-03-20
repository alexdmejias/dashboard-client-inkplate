# OTA Updates Guide

This guide explains how to perform Over-The-Air (OTA) firmware updates on your Inkplate Dashboard Client device.

## Overview

The Inkplate Dashboard Client spends most of its time in deep sleep to conserve battery. This creates a unique challenge for OTA updates since the device needs to be awake to receive firmware updates. This guide provides strategies to successfully update your device wirelessly.

## Prerequisites

- Inkplate device with firmware that includes OTA support (v1.1.0+)
- Device connected to WiFi network
- PlatformIO installed on your computer
- Device's IP address (see "Finding Your Device's IP Address" below)

## Configuration

First, enable OTA in your `config.txt` file on the microSD card:

```json
{
  "server": "https://your-server.com/image",
  "ssid": "your_wifi_ssid",
  "password": "your_wifi_password",
  "sleepTime": 300,
  "otaEnabled": true,
  "otaPort": 3232,
  "otaPassword": "your-secure-password",
  "otaTimeout": 30
}
```

### Configuration Options

- **otaEnabled** (boolean): Enable/disable OTA updates. Set to `true` to enable.
- **otaPort** (integer): Port number for OTA updates. Default is 3232.
- **otaPassword** (string): Optional password to secure OTA updates. Leave empty for no password.
- **otaTimeout** (integer): Seconds to wait for OTA updates after each wake. Default is 30 seconds.

## Update Strategies

Since the device is in deep sleep most of the time, you need to ensure it's awake when you perform the OTA update. Here are three strategies:

### Strategy 1: Debug Mode (Recommended for Updates)

Debug mode keeps the device awake indefinitely, making it ideal for firmware updates.

**Steps:**
1. Connect to the device via serial (115200 baud)
2. Wait for the device to wake up (or press the wake button)
3. During the `debugWindow` period (default 10 seconds), send the command:
   ```
   debug
   ```
4. The device will display "debug mode" and stay awake
5. Perform your OTA update (see "Performing the Update" below)
6. On Inkplate 10: Touch any pad to exit debug mode
7. On Inkplate 10 v2: Send the `restart` command via serial

**Advantages:**
- Device stays awake until you exit debug mode
- No time pressure to complete the update
- Can perform multiple updates in succession

### Strategy 2: OTA Timeout Window

The device waits for OTA updates during the `otaTimeout` window after each wake.

**Steps:**
1. Set `otaTimeout` to a reasonable duration (e.g., 60 seconds) in your config
2. Wait for the device to wake naturally, or press the wake button
3. The device will wait for the configured timeout before proceeding
4. Perform your OTA update within this window

**Advantages:**
- No serial connection required
- Works automatically on every wake cycle

**Disadvantages:**
- Must complete update within the timeout window
- Need to time your update with the wake cycle

### Strategy 3: Extended Sleep Time

Temporarily increase the sleep time to create longer wake windows.

**Steps:**
1. Set a longer `sleepTime` in your config (e.g., 60 seconds instead of 300)
2. Increase `otaTimeout` to match (e.g., 60 seconds)
3. The device will wake more frequently and wait longer for updates
4. Perform your OTA update
5. Restore original `sleepTime` and `otaTimeout` values

## Performing the Update

Once your device is ready to receive updates (using one of the strategies above):

1. Open a terminal on your computer

2. Navigate to your project directory:
   ```bash
   cd /path/to/dashboard-client-inkplate
   ```

3. Build and upload the firmware:
   ```bash
   platformio run --target upload --upload-port <DEVICE_IP_ADDRESS>
   ```
   
   Replace `<DEVICE_IP_ADDRESS>` with your device's actual IP address (e.g., `192.168.1.100`)

4. Monitor the upload progress. You should see output like:
   ```
   Uploading .pio/build/esp32/firmware.bin
   [====                  ] 20%
   ```

5. Wait for the upload to complete. The device will automatically restart with the new firmware.

## Finding Your Device's IP Address

You can find your device's IP address in several ways:

### Method 1: Serial Monitor
1. Connect via serial (115200 baud)
2. Wake the device or wait for it to wake naturally
3. Look for log lines like:
   ```
   Connected to WiFi
   IP address: 192.168.1.100
   ```

### Method 2: Router DHCP List
1. Log into your router's admin interface
2. Look for DHCP client list
3. Find a device named `inkplate-XXXXXXXX` (where X's are hex digits from the MAC address)

### Method 3: Network Scanner
Use a network scanning tool like:
- `nmap -sn 192.168.1.0/24` (Linux/Mac)
- Angry IP Scanner (Windows)
- Fing mobile app (iOS/Android)

Look for a hostname matching `inkplate-XXXXXXXX`

## Securing Your OTA Updates

For production deployments, always set an OTA password:

```json
{
  "otaPassword": "your-very-secure-password-here"
}
```

Without a password, anyone on your network can upload firmware to your device.

## Troubleshooting

### "No response from device"
- Ensure the device is awake (use debug mode or wait for wake cycle)
- Verify the IP address is correct
- Check that `otaEnabled` is `true` in your config
- Ensure device and computer are on the same network

### "Connection refused"
- Check that `otaPort` matches between config and upload command
- Verify firewall settings aren't blocking the connection
- Ensure no other device is using the same IP address

### "Authentication failed"
- Verify the `otaPassword` in your config matches the password used for upload
- If you set a password, use: `platformio run --target upload --upload-port <IP> --upload-flags="--auth=your-password"`

### "Upload interrupted"
- Increase `otaTimeout` to allow more time for the upload
- Use debug mode to eliminate time pressure
- Check network stability and signal strength

### "Device not responding after update"
- This is usually because the new firmware has a compile error
- Connect via serial to see error messages
- Reflash via USB if necessary

## Best Practices

1. **Always test in debug mode first**: Use Strategy 1 for your first OTA update to ensure everything works.

2. **Keep a backup**: Always keep a copy of your working firmware in case you need to revert.

3. **Use passwords**: Protect your device with an OTA password in production.

4. **Plan your updates**: Schedule updates for when you can monitor the device via serial.

5. **Incremental changes**: Make small, testable changes rather than large rewrites.

6. **Document IP addresses**: Keep a record of your devices' IP addresses or use static DHCP reservations.

## Serial Commands for OTA Configuration

You can configure OTA settings via the serial interface:

- `otaEnabled` - Toggle OTA on/off
- `otaPort` - Set OTA port number
- `otaPassword` - Set OTA password
- `otaTimeout` - Set OTA timeout duration
- `save` - Save configuration to SD card
- `current` - Show current configuration

## Example Workflow

Here's a complete example of updating a device:

```bash
# 1. Connect via serial
screen /dev/ttyUSB0 115200

# 2. Wake the device (press button or wait for wake cycle)

# 3. When you see "Waiting X seconds for Serial 'debug' command", type:
debug

# 4. Device enters debug mode and displays "debug mode"

# 5. In another terminal, perform the OTA update:
cd ~/dashboard-client-inkplate
platformio run --target upload --upload-port 192.168.1.100

# 6. Watch the serial monitor for upload progress

# 7. After successful update, exit debug mode:
#    - Inkplate 10: Touch any pad
#    - Inkplate 10 v2: Type "restart" in serial

# 8. Device reboots with new firmware
```

## Additional Resources

- [PlatformIO OTA Documentation](https://docs.platformio.org/en/latest/platforms/espressif32.html#over-the-air-ota-update)
- [ESP32 OTA Updates Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/ota.html)
- [ArduinoOTA Library](https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA)
