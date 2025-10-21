#!/usr/bin/env python3
"""
Example usage script for AMU TWI Passthrough Interface

This script demonstrates how to use the AMUPassthrough class to communicate
with multiple AMU devices connected through a passthrough board.
"""

from amu_passthrough import AMUPassthrough


def example_basic_usage():
    """Basic example of connecting and querying devices"""
    print("=== Basic Usage Example ===")
    
    # Create passthrough interface (auto-detect COM port)
    passthrough = AMUPassthrough()
    
    try:
        # Connect to the passthrough board
        if not passthrough.connect():
            print("Failed to connect to passthrough board")
            return
        
        # Get number of connected devices
        device_count = passthrough.get_device_count()
        print(f"Found {device_count} remote AMU devices")
        
        if device_count > 0:
            # Query first device serial number
            serial = passthrough.query_device(1, "SYST:SER?")
            print(f"Device 1 serial number: {serial}")
            
            # Get detailed info about first device
            device_info = passthrough.get_device_info(1)
            print(f"Device 1 info: {device_info}")
    
    finally:
        # Always disconnect
        passthrough.disconnect()


def example_scan_all_devices():
    """Example of scanning all connected devices"""
    print("\n=== Scan All Devices Example ===")
    
    passthrough = AMUPassthrough()
    
    try:
        if passthrough.connect():
            # Scan all devices and get their information
            all_devices = passthrough.scan_all_devices()
            
            print("\nDevice scan results:")
            for addr, info in all_devices.items():
                print(f"\nDevice {addr}:")
                for key, value in info.items():
                    print(f"  {key}: {value}")
    
    finally:
        passthrough.disconnect()


def example_query_all_devices():
    """Example of sending the same query to all devices"""
    print("\n=== Query All Devices Example ===")
    
    passthrough = AMUPassthrough()
    
    try:
        if passthrough.connect():
            # Query temperature from all devices
            temperatures = passthrough.query_all_devices("SYST:TEMP?")
            
            print("Temperature readings from all devices:")
            for device_addr, temp in temperatures.items():
                print(f"  Device {device_addr}: {temp}°C")
            
            # Query PGA settings from all devices
            voltage_pgas = passthrough.query_all_devices("ADC:CH0:PGA?")

            current_pgas = passthrough.query_all_devices("ADC:CH1:PGA?")
            
            print("Voltage PGA settings from all devices:")
            for device_addr, pga in voltage_pgas.items():
                print(f"  Device {device_addr}: PGA {pga}")

            print("Current PGA settings from all devices:")
            for device_addr, pga in current_pgas.items():
                print(f"  Device {device_addr}: PGA {pga}")

    finally:
        passthrough.disconnect()


def example_specific_com_port():
    """Example using a specific COM port"""
    print("\n=== Specific COM Port Example ===")
    
    # Connect to a specific COM port
    passthrough = AMUPassthrough(com_port="COM4")
    
    try:
        if passthrough.connect():
            print("Connected successfully to COM4")
            
            # Your device communication code here
            device_count = passthrough.get_device_count()
            print(f"Found {device_count} devices on COM4")
    
    finally:
        passthrough.disconnect()


def main():
    """Run all examples"""
    print("AMU TWI Passthrough Interface - Examples")
    print("=" * 50)
    
    try:
        example_basic_usage()
        example_scan_all_devices()
        example_query_all_devices()
        
        # Uncomment to test specific COM port
        # example_specific_com_port()
        
    except KeyboardInterrupt:
        print("\nExamples interrupted by user")
    except Exception as e:
        print(f"Error in examples: {e}")


if __name__ == "__main__":
    main()