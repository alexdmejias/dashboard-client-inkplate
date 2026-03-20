# Building Firmware for ESP Web Tools

This guide explains how to build the firmware.bin file needed for ESP Web Tools flashing.

## Automatic Build (GitHub Actions)

The easiest way to get a firmware.bin file is to use our automated CI/CD:

### Method 1: Download from GitHub Pages
1. Visit the [Web Flasher](https://alexdmejias.github.io/dashboard-client-inkplate/)
2. The latest firmware is automatically built and deployed
3. Flash directly from your browser

### Method 2: Download from Releases
1. Go to the [Releases page](https://github.com/alexdmejias/dashboard-client-inkplate/releases)
2. Download `firmware.bin` from the latest release
3. Use with ESP Web Tools or other flashing methods

### Method 3: Download from CI Artifacts
1. Go to [Actions](https://github.com/alexdmejias/dashboard-client-inkplate/actions)
2. Click on the latest successful workflow run
3. Download the `firmware` artifact
4. Extract `firmware.bin` from the ZIP file

## Manual Build (Local Development)

If you want to build the firmware yourself:

### Prerequisites
- Python 3.7 or later
- PlatformIO Core

### Step 1: Install PlatformIO

```bash
# Install using pip
pip install platformio

# Verify installation
platformio --version
```

### Step 2: Clone the Repository

```bash
git clone https://github.com/alexdmejias/dashboard-client-inkplate.git
cd dashboard-client-inkplate
```

### Step 3: Build the Firmware

```bash
# Build for ESP32
platformio run

# Or build with verbose output
platformio run -v
```

This will:
1. Download required platforms and libraries
2. Compile the code
3. Generate the firmware binary

### Step 4: Locate the Firmware Binary

After a successful build, find the firmware at:

```
.pio/build/esp32/firmware.bin
```

You can also find:
- `firmware.elf` - Executable with debug symbols
- `firmware.map` - Memory map file
- `partitions.bin` - Partition table

### Step 5: Use with ESP Web Tools

To use your custom firmware with ESP Web Tools:

```bash
# Copy firmware to the root directory
cp .pio/build/esp32/firmware.bin ./firmware.bin

# Now you can use flash.html locally
# Open flash.html in Chrome/Edge/Opera
```

## Build Output Explained

### firmware.bin
This is a **merged binary** that contains:
- Bootloader (from ESP32 platform)
- Partition table (default ESP32 partitions)
- Application firmware (your compiled code)

The merged binary can be written directly to flash starting at offset 0x0000, making it perfect for ESP Web Tools.

### Size Information
A typical build produces:
- **firmware.bin**: ~1.5 MB (varies based on features)
- **firmware.elf**: ~3 MB (with debug symbols)

ESP32 flash layout:
```
0x0000 - Bootloader (second stage)
0x8000 - Partition table
0x10000 - Application firmware (OTA0)
0x310000 - Application firmware (OTA1) - for OTA updates
0x610000 - SPIFFS/FAT filesystem (if configured)
```

## Customizing the Build

### Change Build Flags

Edit `platformio.ini`:

```ini
build_flags = 
    -DARDUINO_INKPLATE10V2    # Change to ARDUINO_INKPLATE10 for v1
    -DCORE_DEBUG_LEVEL=1      # 0=None, 1=Error, 2=Warn, 3=Info, 4=Debug, 5=Verbose
    -DBOARD_HAS_PSRAM
    -mfix-esp32-psram-cache-issue
```

### Add Custom Features

1. Modify source files in `src/`
2. Add new libraries to `lib_deps` in `platformio.ini`
3. Rebuild with `platformio run`

### Clean Build

To start fresh:

```bash
# Clean build files
platformio run --target clean

# Clean everything including downloaded packages
rm -rf .pio
platformio run
```

## Testing Your Firmware

### Option 1: Test via Serial (Recommended)

Before using ESP Web Tools, test via USB:

```bash
# Flash via USB
platformio run --target upload

# Monitor serial output
platformio device monitor
```

### Option 2: Test with ESP Web Tools

1. Copy `firmware.bin` to the repository root
2. Open `flash.html` in Chrome/Edge/Opera
3. Connect your device via USB
4. Flash and verify

### Option 3: Test via OTA

If you already have OTA-enabled firmware:

```bash
# Flash via network
platformio run --target upload --upload-port <DEVICE_IP>
```

## Troubleshooting

### "xtensa-esp32-elf-gcc not found"
The ESP32 toolchain is downloading. Wait for it to complete or check your internet connection.

### "Library not found"
PlatformIO is downloading dependencies. This happens on first build. Wait for completion.

### Build fails with memory errors
Your firmware may be too large. Check:
```bash
# View size of firmware sections
pio size -v
```

Consider:
- Reducing debug level: `-DCORE_DEBUG_LEVEL=0`
- Disabling unused features
- Using compiler optimization flags

### firmware.bin not generated
Build failed. Check the error messages:
```bash
# Build with verbose output
platformio run -v
```

Common issues:
- Syntax errors in code
- Missing libraries
- Incompatible library versions

## CI/CD Pipeline

Our GitHub Actions workflow automatically:

1. **On every push to main**:
   - Builds firmware
   - Deploys to GitHub Pages
   - Makes it available at the web flasher

2. **On pull requests**:
   - Builds firmware to verify changes
   - Makes artifacts available for download

3. **On version tags** (e.g., `v1.0.0`):
   - Builds firmware
   - Creates a GitHub Release
   - Attaches firmware.bin to the release

### Triggering a Build

```bash
# Method 1: Push to main
git push origin main

# Method 2: Create a tag for release
git tag v1.0.0
git push origin v1.0.0

# Method 3: Manual trigger (via GitHub UI)
# Go to Actions > Build Firmware and Deploy > Run workflow
```

## Advanced: Build Variants

To create multiple firmware variants:

### Edit platformio.ini

```ini
[platformio]
default_envs = inkplate10, inkplate10v2

[env:inkplate10]
platform = https://github.com/platformio/platform-espressif32.git
framework = arduino
board = esp32dev
build_flags = -DARDUINO_INKPLATE10

[env:inkplate10v2]
platform = https://github.com/platformio/platform-espressif32.git
framework = arduino
board = esp32dev
build_flags = -DARDUINO_INKPLATE10V2
```

### Build all variants

```bash
platformio run

# Outputs:
# .pio/build/inkplate10/firmware.bin
# .pio/build/inkplate10v2/firmware.bin
```

## Next Steps

- [Deploy to GitHub Pages](./GITHUB_PAGES_SETUP.md)
- [OTA Updates Guide](./docs/OTA_GUIDE.md)
- [ESP Web Tools Guide](./docs/ESP_WEB_TOOLS_GUIDE.md)

## References

- [PlatformIO Documentation](https://docs.platformio.org/)
- [ESP32 Build System](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html)
- [ESP Web Tools](https://esphome.github.io/esp-web-tools/)
