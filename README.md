# AMU Library

A C/C++ library for communicating with AMU (Aerospace Measurement Unit) devices over the I2C or USB interface.

## Overview

This Arduino-compatible library provides a comprehensive interface for controlling and reading data from AMU devices via I2C communication. The library supports multiple AMU devices on the same I2C bus and includes SCPI-based communication capabilities.

## Documentation and API Reference

Doxygen generated [documentation](https://the-aerospace-corporation.github.io/amulib/html/index.html).

## Features

- **I2C Communication**: Full support for communicating with AMU devices over the I2C bus
- **Multi-device Support**: Connect and control multiple AMU devices on the same bus
- **Arduino Compatible**: Designed to work seamlessly with Arduino and PlatformIO projects
- **SCPI Support**: Built-in SCPI (Standard Commands for Programmable Instruments) protocol support
- **Measurement Capabilities**:
  - Voltage measurements
  - Current measurements
  - Temperature sensor readings (multiple sensors)
  - Programmable gain amplifier (PGA) control
- **Device Control**:
  - LED color and pattern control
  - Channel activation/deactivation
  - Sleep mode management
  - Device information retrieval (serial number, firmware version, notes)

## Installation

### Arduino IDE
1. Download this library as a ZIP file
2. In Arduino IDE, go to Sketch → Include Library → Add .ZIP Library
3. Select the downloaded ZIP file

### PlatformIO
Add the following to your `platformio.ini` file:
```ini
lib_deps = 
    https://github.com/the-aerospace-corporation/amulib.git
```

## Quick Start

```cpp
#include <Wire.h>
#include <amulib.h>

// I2C transfer function for Arduino Wire library
int amu_wire_transfer(TwoWire* wire, uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read) {
    uint8_t packetNum = 0;
    
    if (read) {
        if (len > 0) {
            wire->beginTransmission(address);
            wire->write(reg);
            wire->endTransmission();
            while (len > BUFFER_LENGTH) {
                wire->requestFrom(address, (uint8_t)BUFFER_LENGTH, (uint8_t)0);
                if (wire->available()) {
                    wire->readBytes(&data[packetNum * BUFFER_LENGTH], BUFFER_LENGTH);
                    packetNum++;
                    len -= BUFFER_LENGTH;
                }
            }
            wire->requestFrom(address, (uint8_t)len);
            if (wire->available())
                wire->readBytes(&data[packetNum * BUFFER_LENGTH], len);
        }
        else {
            wire->beginTransmission(address);
            return wire->endTransmission();
        }
    }
    else {
        wire->beginTransmission(address);
        wire->write(reg);
        while (len > BUFFER_LENGTH) {
            wire->write(&data[packetNum * BUFFER_LENGTH], BUFFER_LENGTH);
            len -= BUFFER_LENGTH;
            packetNum++;
        }
        wire->write(&data[packetNum * BUFFER_LENGTH], len);
        wire->endTransmission();
    }
    
    return 0;
}

// Wrapper function for the AMU library
int8_t twi_transfer(uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read) {
    return (int8_t)amu_wire_transfer(&Wire, address, reg, data, len, read);
}

AMU amu;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(400000);
    
    // Initialize AMU device at I2C address 0x0B with transfer function
    amu.begin(0x0B, twi_transfer);
    
    // Wait for device to be ready
    if (amu.waitUntilReady(5000) == 0) {
        Serial.println("AMU device ready!");
    }
}

void loop() {
    // Read voltage
    float voltage = amu.measureVoltage();
    Serial.print("Voltage: ");
    Serial.println(voltage);
    
    // Read current
    float current = amu.measureCurrent();
    Serial.print("Current: ");
    Serial.println(current);
    
    // Read temperature
    float temp = amu.measureTSensor();
    Serial.print("Temperature: ");
    Serial.println(temp);
    
    delay(1000);
}
```

## Examples

The library includes several example projects:

- **simple**: Basic AMU communication and measurement example
- **iv_sweep**: Current-voltage sweep measurements
- **twi_passthrough**: I2C passthrough functionality

Each example includes its own `platformio.ini` configuration and can be built independently.

## API Reference

### Initialization
- `begin(uint8_t twiAddress, amu_transfer_fptr_t i2c_transfer_func)` - Initialize with I2C address and custom I2C transfer function (required for Arduino/PlatformIO projects)
- `waitUntilReady(uint32_t timeout)` - Wait for device to be ready

### Measurements
- `measureVoltage()` - Read voltage measurement
- `measureCurrent()` - Read current measurement
- `measureTSensor()` - Read primary temperature sensor
- `measureTSensor0()` - Read temperature sensor 0
- `measureTSensor1()` - Read temperature sensor 1
- `measureTSensor2()` - Read temperature sensor 2

### Device Control
- `setActiveChannels(uint16_t channels)` - Set active measurement channels
- `setLEDcolor(float red, float grn, float blu)` - Set LED color
- `setLEDmode(amu_led_pattern_t mode)` - Set LED pattern mode
- `sleep()` - Put device into sleep mode
- `getPGA(AMU_ADC_CH_t channel)` - Get programmable gain amplifier setting

### Device Information
- `readSerialStr()` - Read device serial number
- `readFirmwareStr()` - Read firmware version
- `readNotes(char* notes)` - Read device notes

## Hardware Requirements

- Arduino or compatible microcontroller with I2C support
- AMU device(s)
- I2C pull-up resistors (typically 4.7kΩ)

## License

This library is released under the MIT License. See the LICENSE file for details.

## Author

Colin Mann - [Aerospace Corporation](https://aero.org)

## Version

Current version: 2.1

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.
