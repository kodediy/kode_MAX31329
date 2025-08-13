# kode_MAX31329

An Arduino library for the MAX31329 real-time clock (RTC) over I2C, specifically designed for ESP32-S3 processors.

## Features

- **Fluent API**: Easy-to-use time interface with `rtc.t.year = 2025` syntax
- **Multiple initialization options**: Flexible `begin()` methods for different use cases
- **Complete RTC functionality**: Time, alarms, timer, power management, and NVRAM
- **ESP32-S3 optimized**: Tailored for kode dot hardware platform
- **Apache 2.0 licensed**: Commercial-friendly open source license

## Installation

### Arduino IDE

1. Download the library as a ZIP file
2. Open Arduino IDE
3. Go to **Sketch** → **Include Library** → **Add .ZIP Library...**
4. Select the downloaded ZIP file
5. The library will be installed and ready to use

### PlatformIO

Add to your `platformio.ini`:

```ini
lib_deps = 
    https://github.com/yourusername/kode_MAX31329.git
```

## Quick Start

```cpp
#include <kode_MAX31329.h>

MAX31329 rtc;

void setup() {
    Serial.begin(115200);
    
    // Initialize with custom I2C pins (most common for ESP32-S3)
    rtc.begin(48, 47);  // SDA=48, SCL=47
    
    // Set current time using fluent API
    rtc.t.year = 2025;
    rtc.t.month = 11;
    rtc.t.day = 24;
    rtc.t.hour = 15;
    rtc.t.minute = 30;
    rtc.t.second = 0;
    rtc.writeTime();
}

void loop() {
    // Read current time
    if (rtc.readTime()) {
        Serial.printf("Time: %04d-%02d-%02d %02d:%02d:%02d\n",
            rtc.t.year, rtc.t.month, rtc.t.day,
            rtc.t.hour, rtc.t.minute, rtc.t.second);
    }
    
    delay(1000);
}
```

## API Reference

### Initialization

```cpp
// Most common: custom I2C pins
rtc.begin(48, 47);              // SDA=48, SCL=47, 400kHz
rtc.begin(48, 47, 100000);      // SDA=48, SCL=47, 100kHz

// Use Wire to set pins
Wire.begin(48,47)
rtc.begin();                    // 400kHz
rtc.begin(100000);              // 100kHz

// Custom Wire bus (advanced)
TwoWire myWire(1);
rtc.begin(myWire, 48, 47);
```

### Time Operations

#### Fluent API (Recommended)

```cpp
// Set time
rtc.t.year = 2025;
rtc.t.month = 11;
rtc.t.day = 24;
rtc.t.hour = 15;
rtc.t.minute = 30;
rtc.t.second = 0;
rtc.t.dayOfWeek = 0;  // 0=Sunday, 1=Monday, ..., 6=Saturday
rtc.writeTime();

// Read time
rtc.readTime();
Serial.printf("Year: %d, Month: %d, Day: %d\n", 
    rtc.t.year, rtc.t.month, rtc.t.day);
```

#### Alternative APIs

```cpp
// Direct parameters
rtc.writeTime(2025, 11, 24, 15, 30, 0, 0);

// Using struct tm
struct tm timeinfo;
rtc.readTime(timeinfo);
rtc.writeTime(timeinfo);

// Individual parameters by reference
int year, month, day, hour, minute, second, dayOfWeek;
rtc.readTime(year, month, day, hour, minute, second, dayOfWeek);
```

### Status and Interrupts

```cpp
// Check connection
if (rtc.isConnected()) {
    Serial.println("RTC connected");
}

// Read status register
uint8_t status;
rtc.readStatus(status);

// Clear status flags
rtc.clearStatus();

// Enable/disable interrupts
rtc.enableInterrupts(MAX31329_INT_A1IE);   // Alarm 1
rtc.enableInterrupts(MAX31329_INT_A2IE);   // Alarm 2
rtc.enableInterrupts(MAX31329_INT_TIE);    // Timer
rtc.disableInterrupts(MAX31329_INT_A1IE);
```

### RTC Control

```cpp
// Oscillator control
rtc.startRTC();
rtc.stopRTC();

// Software reset
rtc.assertReset();
rtc.releaseReset();
```

### Clock Output (CLKO)

```cpp
// Enable clock output with frequency selection
rtc.clkoEnable(0);  // 1Hz
rtc.clkoEnable(1);  // 50Hz
rtc.clkoEnable(2);  // 60Hz
rtc.clkoEnable(3);  // 32kHz

// Disable clock output
rtc.clkoDisable();

// Disable clock input
rtc.clkinDisable();
```

### Timer Functions

```cpp
// Configure timer
rtc.timerConfigure(16, true, 3);  // initial=16, repeat=true, freq=16Hz

// Timer control
rtc.timerStart();
rtc.timerPause();
rtc.timerContinue();
rtc.timerStop();

// Read timer value
uint8_t value;
rtc.timerRead(value);
```

### Power Management

```cpp
// Set power fail threshold (0-3)
rtc.setPowerFailThreshold(2);

// Select power supply
rtc.selectSupply(0);  // Auto
rtc.selectSupply(1);  // VCC
rtc.selectSupply(2);  // VBACKUP

// Trickle charger
rtc.trickleEnable(0x05);  // Enable with path 0x05
rtc.trickleDisable();
```

### NVRAM Access

```cpp
// Write to NVRAM
uint8_t data[] = {0x01, 0x02, 0x03};
rtc.writeRam(0, data, sizeof(data));

// Read from NVRAM
uint8_t buffer[3];
rtc.readRam(0, buffer, sizeof(buffer));
```

### Low-Level Register Access

```cpp
// Read registers
uint8_t buffer[4];
rtc.readBytes(MAX31329_REG_SECONDS, buffer, 4);

// Write registers
uint8_t data[] = {0x00, 0x00, 0x12, 0x34};
rtc.writeBytes(MAX31329_REG_SECONDS, data, 4);
```

## Examples

The library includes several examples:

- **Time**: Basic time reading and writing
- **Alarm1**: Alarm 1 configuration and interrupt handling
- **Alarm2**: Alarm 2 configuration and interrupt handling  
- **Timer**: Timer configuration and interrupt handling

## Register Constants

All MAX31329 registers and bit fields are defined with descriptive names:

```cpp
// Status register bits
MAX31329_STATUS_A1F      // Alarm 1 Flag
MAX31329_STATUS_A2F      // Alarm 2 Flag  
MAX31329_STATUS_TIF      // Timer Interrupt Flag

// Interrupt enable bits
MAX31329_INT_A1IE        // Alarm 1 Interrupt Enable
MAX31329_INT_A2IE        // Alarm 2 Interrupt Enable
MAX31329_INT_TIE         // Timer Interrupt Enable

// And many more...
```

## Error Handling

All methods return `bool` to indicate success (`true`) or failure (`false`):

```cpp
if (!rtc.begin(48, 47)) {
    Serial.println("Failed to initialize RTC");
    return;
}

if (!rtc.writeTime()) {
    Serial.println("Failed to set time");
}
```

## License

This library is licensed under the Apache License 2.0. See the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## Changelog

### v1.0.0
- Initial release
- Fluent API with `rtc.t.year = 2025` syntax
- Multiple `begin()` overloads for flexible initialization
- Complete MAX31329 functionality
- ESP32-S3 optimization
- Apache 2.0 license

## Support

For questions and support, please open an issue on the GitHub repository.
