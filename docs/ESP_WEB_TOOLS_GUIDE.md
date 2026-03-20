# ESP Web Tools Flashing Guide

This guide explains how to flash the Inkplate Dashboard Client firmware using ESP Web Tools - a web-based flashing tool that works directly in your browser.

## What is ESP Web Tools?

ESP Web Tools is a web-based tool that allows you to flash ESP32 and ESP8266 devices directly from your web browser, without needing to install any software or command-line tools. It uses the WebSerial API to communicate with your device over USB.

## Prerequisites

### Browser Requirements
ESP Web Tools requires a browser that supports the WebSerial API:
- ✅ Chrome (version 89+)
- ✅ Edge (version 89+)
- ✅ Opera (version 75+)
- ❌ Firefox (not supported)
- ❌ Safari (not supported)

### Hardware Requirements
- Inkplate 10 or Inkplate 10 v2 device
- USB cable (typically USB-C or Micro-USB depending on your model)
- Computer with a USB port

## Flashing Steps

### Option 1: Using the Hosted Flasher (Easiest)

The easiest way to flash your device is using our GitHub Pages hosted flasher:

1. **Visit the flasher page**
   - Go to [https://alexdmejias.github.io/dashboard-client-inkplate/](https://alexdmejias.github.io/dashboard-client-inkplate/)
   - This page is automatically updated with the latest firmware on every commit to main

2. **Connect your device**
   - Connect your Inkplate device to your computer via USB
   - Ensure the device is powered on

3. **Start the flashing process**
   - Click the "Connect and Install" button
   - A dialog will appear showing available serial ports
   - Select your Inkplate device from the list (usually shows as "USB Serial" or similar)
   - Click "Connect"

4. **Install the firmware**
   - Click "Install" in the ESP Web Tools interface
   - The tool will erase the device and flash the new firmware
   - Wait for the process to complete (usually takes 1-2 minutes)

5. **Complete!**
   - Once finished, the device will automatically reboot
   - You can disconnect from the web interface

### Option 2: Using the Local HTML File

If you prefer to use a local copy or want to test custom firmware:

1. **Open the flasher page**
   - Locate `flash.html` in the root of this repository
   - Open it in Chrome, Edge, or Opera browser
   - You can open it directly from your file system (no web server needed)

2. **Prepare custom firmware (optional)**
   - If using custom firmware, build it first:
     ```bash
     platformio run
     cp .pio/build/esp32/firmware.bin ./firmware.bin
     ```
   - The local `flash.html` will use the `firmware.bin` in the same directory

3. **Connect and flash**
   - Connect your Inkplate device via USB
   - Click "Connect and Install"
   - Select your device and follow the prompts
   - Wait for flashing to complete

## Post-Flashing Setup

After flashing the firmware, you need to configure the device:

1. **Prepare your microSD card**
   - Format a microSD card as FAT32
   - Create a file named `config.txt` in the root of the card

2. **Configure the device**
   - Copy the contents from `example-config.json` in this repository
   - Modify the values for your setup:
     - `server`: Your image server URL
     - `ssid`: Your WiFi network name
     - `password`: Your WiFi password
     - Other settings as needed

3. **Insert the SD card**
   - Insert the configured microSD card into your Inkplate device

4. **Power cycle**
   - Disconnect and reconnect the USB cable, or press the reset button
   - The device should connect to WiFi and start displaying images

## Example Configuration

Here's a minimal `config.txt` file to get started:

```json
{
  "server": "https://your-server.com/image.png",
  "ssid": "YourWiFiNetwork",
  "password": "YourWiFiPassword",
  "sleepTime": 300,
  "wifiTimeout": 30,
  "httpTimeout": 15,
  "showDebug": true,
  "wakeButtonPin": 36,
  "debugWindow": 10,
  "otaEnabled": false,
  "otaPort": 3232,
  "otaPassword": "",
  "otaTimeout": 30
}
```

**Note:** Set `showDebug` to `true` for your first run to see the configuration on the display.

## Troubleshooting

### "No compatible device found"
- Ensure your device is connected via USB
- Try a different USB cable (some cables are power-only)
- Check that the device is powered on
- Try a different USB port

### "WebSerial not supported"
- You're using an incompatible browser
- Use Chrome, Edge, or Opera instead
- Update your browser to the latest version

### "Failed to open serial port"
- The port may be in use by another application
- Close any serial monitors or Arduino IDE instances
- Unplug and replug the USB cable
- Try restarting your browser

### "Installation failed"
- Ensure the firmware file (`firmware.bin`) is in the correct location
- Check that you have a stable connection
- Try the process again

### Device not working after flash
1. Connect via serial monitor (115200 baud) to see error messages
2. Verify your `config.txt` file is correctly formatted
3. Ensure the microSD card is properly inserted
4. Try reformatting the SD card and recreating `config.txt`

### Permission denied (Linux)
Add your user to the `dialout` group:
```bash
sudo usermod -a -G dialout $USER
```
Then log out and log back in.

## Building Custom Firmware for ESP Web Tools

If you want to build your own firmware binary for ESP Web Tools:

1. **Build the firmware with PlatformIO:**
   ```bash
   cd /path/to/dashboard-client-inkplate
   platformio run
   ```

2. **Locate the firmware binary:**
   - The compiled firmware will be in `.pio/build/esp32/firmware.bin`

3. **Copy to the project root:**
   ```bash
   cp .pio/build/esp32/firmware.bin ./firmware.bin
   ```

4. **Update manifest.json if needed:**
   - The `manifest.json` file points to `firmware.bin`
   - You can add multiple firmware variants if needed

5. **Test with ESP Web Tools:**
   - Open `flash.html` in your browser
   - The tool will use the new `firmware.bin` file

## Advanced: Hosting Your Own Flasher

To host the flasher on your own web server:

1. **Copy these files to your web server:**
   - `flash.html`
   - `manifest.json`
   - `firmware.bin`

2. **Ensure HTTPS:**
   - WebSerial requires HTTPS (except for localhost)
   - Use a service like GitHub Pages, Netlify, or your own HTTPS-enabled server

3. **Update manifest.json:**
   - Adjust the `path` in `manifest.json` if your firmware.bin is in a different location

4. **Share the URL:**
   - Users can now flash their devices by visiting your hosted URL

## Security Considerations

- ESP Web Tools only works over HTTPS (except localhost)
- The firmware binary is transferred directly from your computer to the device
- No data is sent to third-party servers during the flashing process
- Users should only flash firmware from trusted sources

## Alternative: Traditional USB Flashing

If ESP Web Tools doesn't work for you, you can always use the traditional PlatformIO method:

```bash
# Install PlatformIO
pip install platformio

# Flash via USB
cd /path/to/dashboard-client-inkplate
platformio run --target upload
```

See the main README for more details on PlatformIO flashing.

## Additional Resources

- [ESP Web Tools Official Site](https://esphome.github.io/esp-web-tools/)
- [WebSerial API Documentation](https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [Inkplate Documentation](https://inkplate.readthedocs.io/)

## Feedback

If you encounter any issues with ESP Web Tools flashing, please:
1. Check the troubleshooting section above
2. Ensure you're using a compatible browser
3. Report issues on the GitHub repository with details about your setup
