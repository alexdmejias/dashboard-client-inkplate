# Quick Start Guide

Get your Inkplate Dashboard Client up and running in minutes!

## 🚀 Quick Flash (Easiest Method)

1. **Visit the Web Flasher**
   - Go to: [https://alexdmejias.github.io/dashboard-client-inkplate/](https://alexdmejias.github.io/dashboard-client-inkplate/)
   - Works in Chrome, Edge, or Opera browsers

2. **Connect Your Device**
   - Plug your Inkplate into your computer via USB
   - Click "Connect and Install"
   - Select your device from the list
   - Click "Install"

3. **Configure**
   - Format a microSD card (FAT32)
   - Create `config.txt` file (see below)
   - Insert card into Inkplate
   - Power cycle the device

That's it! Your device should now connect to WiFi and display images.

## 📝 Minimal Configuration

Create a `config.txt` file on your microSD card with at least these fields:

```json
{
  "server": "https://your-server.com/image.png",
  "ssid": "YourWiFiName",
  "password": "YourWiFiPassword",
  "sleepTime": 300
}
```

**Replace:**
- `server` - URL that returns a PNG image
- `ssid` - Your WiFi network name
- `password` - Your WiFi password
- `sleepTime` - Seconds between refreshes (300 = 5 minutes)

## 🔧 Optional Configuration

Add these for more features:

```json
{
  "server": "https://your-server.com/image.png",
  "ssid": "YourWiFiName",
  "password": "YourWiFiPassword",
  "sleepTime": 300,
  "showDebug": true,
  "wakeButtonPin": 36,
  "debugWindow": 10,
  "otaEnabled": false,
  "wifiTimeout": 30,
  "httpTimeout": 15
}
```

**Options:**
- `showDebug` (true/false) - Show config info on screen
- `wakeButtonPin` (number) - GPIO pin for wake button (default: 36)
- `debugWindow` (seconds) - Time to wait for "debug" command at boot
- `otaEnabled` (true/false) - Enable wireless firmware updates
- `wifiTimeout` (seconds) - Max time to wait for WiFi connection
- `httpTimeout` (seconds) - Max time to wait for image download

## 📡 Wireless Updates (OTA)

Want to update firmware without USB? Enable OTA:

```json
{
  "otaEnabled": true,
  "otaPort": 3232,
  "otaPassword": "your-secure-password",
  "otaTimeout": 30
}
```

Then update via network:
```bash
platformio run --target upload --upload-port <DEVICE_IP>
```

See [OTA Guide](docs/OTA_GUIDE.md) for details on dealing with deep sleep.

## 🔘 Wake Button Setup

Connect a button between GPIO pin (default 36) and GND:

1. Button connects GPIO → GND
2. Press button to wake from sleep
3. Useful for triggering OTA updates

Supported pins: 0, 2, 4, 12-15, 25-27, 32-39

## 🖥️ Serial Commands

Connect via serial (115200 baud) to configure without removing SD card:

```
debug       - Enter debug mode (stay awake)
ssid        - Set WiFi SSID
password    - Set WiFi password
server      - Set server URL
sleepTime   - Set sleep duration
otaEnabled  - Toggle OTA on/off
save        - Save configuration to SD card
current     - Show current settings
restart     - Restart device
help        - Show all commands
```

## 🆘 Troubleshooting

### Device not connecting to WiFi
- Check SSID and password in config.txt
- Ensure WiFi is 2.4GHz (ESP32 doesn't support 5GHz)
- Try increasing `wifiTimeout` to 60 seconds

### No image displaying
- Verify server URL returns a valid PNG image
- Check serial output for error messages
- Try increasing `httpTimeout`
- Enable `showDebug` to see connection status

### Can't flash via web browser
- Use Chrome, Edge, or Opera (not Firefox or Safari)
- Check USB cable (must support data, not just power)
- Try a different USB port
- Install USB drivers if needed

### Device won't wake from sleep
- Verify sleep time isn't too short (minimum ~20 seconds recommended)
- Check wake button wiring (button → GND)
- Verify GPIO pin is supported for wake (see list above)

### OTA updates not working
- Device must be awake to receive updates
- Use "debug" mode to keep device awake
- Or time your update with the wake cycle
- See [OTA Guide](docs/OTA_GUIDE.md) for strategies

## 📚 More Documentation

- 📘 [Building Firmware](docs/BUILDING_FIRMWARE.md) - Build from source
- 📗 [GitHub Pages Setup](docs/GITHUB_PAGES_SETUP.md) - Host your own flasher
- 📕 [OTA Updates](docs/OTA_GUIDE.md) - Wireless firmware updates
- 📙 [ESP Web Tools](docs/ESP_WEB_TOOLS_GUIDE.md) - Web flashing details

## 💡 Tips

1. **First setup**: Enable `showDebug: true` to see what's happening
2. **Testing**: Use short sleep times (60 seconds) during setup
3. **Production**: Use longer sleep times (300-3600 seconds) to save battery
4. **Updates**: Keep OTA disabled unless actively updating
5. **Reliability**: Use static DHCP reservations for OTA updates

## 🎯 Example Servers

Your server just needs to return a PNG image. Examples:

### Simple Static Image
```
https://example.com/dashboard.png
```

### Dynamic Image
Use any backend that generates images:
- Node.js + Canvas
- Python + Pillow
- PHP + GD
- Any image generation service

The Inkplate will fetch and display whatever PNG your server returns!

## 🔗 Resources

- [GitHub Repository](https://github.com/alexdmejias/dashboard-client-inkplate)
- [Web Flasher](https://alexdmejias.github.io/dashboard-client-inkplate/)
- [Inkplate Documentation](https://inkplate.readthedocs.io/)
- [Dashboard Server Example](https://github.com/alexdmejias/dashboard-server)

---

**Need help?** Open an issue on [GitHub](https://github.com/alexdmejias/dashboard-client-inkplate/issues)
