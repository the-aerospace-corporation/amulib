# AMU TWI Passthrough Interface

This module provides a Python interface for communicating with multiple AMU devices connected through a TWI (Two-Wire Interface) passthrough board. The passthrough board acts as a bridge, allowing you to communicate with remote AMU devices using SCPI addressing.

## Overview

The passthrough board setup allows:
- Connection to N remote AMU devices through a single serial interface
- SCPI addressing to target specific devices: `COMMAND (@device_address)`
- Automatic device discovery and enumeration
- Centralized communication management

## Files

- `amu_passthrough.py` - Main passthrough interface class
- `amu_passthrough_examples.py` - Usage examples and demonstrations
- `amu_passthrough_readme.md` - This documentation

## Quick Start

```python
from amu_passthrough import AMUPassthrough

# Connect to passthrough board
passthrough = AMUPassthrough()
if passthrough.connect():
    # Query device 2 serial number
    serial = passthrough.query_device(2, "SYST:SER?")
    print(f"Device 2 serial: {serial}")
    
    passthrough.disconnect()
```

## SCPI Addressing Format

Commands are addressed using the format: `COMMAND (@device_address)`

Examples:
- `SYST:SER? (@1)` - Get serial number from device 1
- `SYST:TEMP? (@3)` - Get temperature from device 3
- `ADC:VOLT:PGA? (@2)` - Get voltage PGA setting from device 2

## Device Numbering

- Device addresses are 1-based (1, 2, 3, ...)
- The passthrough board itself is NOT included in the addressable devices
- If `SYST:TWI:NUM?` returns 5, there are 4 addressable remote devices (1-4)

## Command Line Usage

### Basic Connection and Device Count
```bash
python amu_passthrough.py
```

### Scan All Devices
```bash
python amu_passthrough.py --scan
```

### Query Specific Device
```bash
python amu_passthrough.py --device 2 --command "SYST:SER?"
```

### Use Specific COM Port
```bash
python amu_passthrough.py --port COM4 --scan
```

## API Reference

### AMUPassthrough Class

#### Constructor
```python
AMUPassthrough(com_port=None)
```
- `com_port`: Optional COM port specification. If None, auto-detection is used.

#### Methods

##### connect()
Connects to the passthrough board and discovers connected devices.
- Returns: `bool` - True if successful

##### disconnect()
Disconnects from the passthrough board.

##### get_device_count()
Returns the number of addressable remote devices.
- Returns: `int` - Number of remote devices

##### query_device(device_address, command, timeout=1.0)
Send a query to a specific device.
- `device_address`: Target device address (1-based)
- `command`: SCPI command to send
- `timeout`: Query timeout in seconds
- Returns: `str` - Device response, or None if error

##### send_device(device_address, command)
Send a command to a specific device (no response expected).
- `device_address`: Target device address (1-based)
- `command`: SCPI command to send
- Returns: `bool` - True if successful

##### query_all_devices(command, timeout=1.0)
Send the same query to all connected devices.
- `command`: SCPI command to send
- `timeout`: Query timeout per device
- Returns: `dict` - Mapping of device address to response

##### get_device_info(device_address)
Get basic information about a specific device.
- `device_address`: Target device address
- Returns: `dict` - Device information (serial, firmware, hardware, temperature)

##### scan_all_devices()
Scan all connected devices and return their information.
- Returns: `dict` - Mapping of device address to device info

## Example Usage Patterns

### Device Discovery
```python
passthrough = AMUPassthrough()
if passthrough.connect():
    count = passthrough.get_device_count()
    print(f"Found {count} devices")
    
    # Get serial numbers of all devices
    serials = passthrough.query_all_devices("SYST:SER?")
    for addr, serial in serials.items():
        print(f"Device {addr}: {serial}")
```

### Configuration Management
```python
# Set voltage PGA on all devices
for device_addr in range(1, passthrough.get_device_count() + 1):
    passthrough.send_device(device_addr, "ADC:VOLT:PGA 3")

# Verify settings
pga_settings = passthrough.query_all_devices("ADC:VOLT:PGA?")
```

### Individual Device Control
```python
# Work with a specific device
device_addr = 2
temp = passthrough.query_device(device_addr, "SYST:TEMP?")
firmware = passthrough.query_device(device_addr, "SYST:FIRM?")

print(f"Device {device_addr} - Temp: {temp}°C, FW: {firmware}")
```

## Error Handling

The interface includes comprehensive error handling:
- Connection failures are reported clearly
- Invalid device addresses are caught
- Command timeouts are handled gracefully
- Device communication errors return None or False

```python
response = passthrough.query_device(1, "SYST:SER?")
if response is None:
    print("Error communicating with device")
else:
    print(f"Serial: {response}")
```

## Requirements

- Python 3.6+
- `amu.py` module (AMU communication library)
- AMU passthrough board with TWI passthrough firmware
- Connected AMU devices

## Troubleshooting

### Connection Issues
- Verify COM port is correct
- Check that passthrough board is in TWI passthrough mode
- Ensure proper power and connections to remote devices

### Device Not Found
- Verify device addresses are 1-based
- Check that `SYST:TWI:NUM?` returns expected count
- Ensure remote devices are powered and responding

### Communication Timeouts
- Increase timeout values for slow-responding devices
- Check I2C bus integrity and connections
- Verify remote device firmware is responsive

## Notes

- Device addresses start at 1, not 0
- The passthrough board itself is not addressable
- All commands are passed through with (@address) appending
- Device discovery happens automatically on connection
- Always call `disconnect()` when finished