#!/usr/bin/env python3
"""
AMU TWI Passthrough Interface

This script provides a class for communicating with multiple AMU devices
connected through a TWI passthrough board. The passthrough board allows
communication with N remote AMU devices using SCPI addressing.

Example usage:
    passthrough = AMUPassthrough()
    if passthrough.connect():
        device_count = passthrough.get_device_count()
        print(f"Found {device_count} remote AMU devices")
        
        # Query device 2 serial number
        serial = passthrough.query_device(2, "SYST:SER?")
        print(f"Device 2 serial: {serial}")
        
        passthrough.disconnect()

Author: Generated for AMU library
"""

import sys
import time
import argparse
from amu import amu as AMU


class AMUPassthrough:
    """Class for communicating with AMU devices through a TWI passthrough board"""
    
    def __init__(self, com_port=None):
        """
        Initialize the passthrough interface
        
        Args:
            com_port (str, optional): COM port for the passthrough board.
                                    If None, auto-detect will be used.
        """
        self.com_port = com_port
        self.amu = None
        self.device_count = 0
        self.connected = False
        
    def connect(self):
        """
        Connect to the passthrough board and determine device count
        
        Returns:
            bool: True if connection successful, False otherwise
        """
        try:
            print("Connecting to AMU passthrough board...")
            self.amu = AMU(com_port=self.com_port)
            
            if not self.amu.available:
                if self.com_port:
                    print(f"AMU passthrough board not available on {self.com_port}")
                else:
                    print("AMU passthrough board not available. Check connection.")
                return False
            
            # Clear any previous commands
            self.amu.send("*CLS")
            
            # Get firmware version to confirm connection
            firmware = self.amu.query("SYST:FIRM?")
            print(f"Connected to passthrough board with firmware: {firmware}")
            
            # Get the number of devices
            self.device_count = self._get_device_count()
            if self.device_count > 0:
                print(f"Found {self.device_count} remote AMU devices connected")
                self.connected = True
                return True
            else:
                print("No remote AMU devices found")
                return False
                
        except Exception as e:
            print(f"Error connecting to passthrough board: {e}")
            return False
    
    def disconnect(self):
        """Disconnect from the passthrough board"""
        if self.amu:
            try:
                self.amu.disconnect()
                print("Disconnected from passthrough board")
            except Exception as e:
                print(f"Error during disconnect: {e}")
        
        self.connected = False
        self.amu = None
        self.device_count = 0
    
    def _get_device_count(self):
        """
        Get the number of remote AMU devices connected
        
        Returns:
            int: Number of remote devices (excluding the passthrough board itself)
        """
        try:
            # Query the total number of devices (including passthrough board)
            total_devices = int(self.amu.query("SYST:TWI:NUM?"))
            
            # Subtract 1 because the passthrough board counts itself
            remote_devices = total_devices - 1
            
            return max(0, remote_devices)  # Ensure we don't return negative
            
        except Exception as e:
            print(f"Error getting device count: {e}")
            return 0
    
    def get_device_count(self):
        """
        Get the cached number of remote devices
        
        Returns:
            int: Number of remote AMU devices
        """
        return self.device_count
    
    def query_device(self, device_address, command):
        """
        Send a query to a specific device through the passthrough board
        
        Args:
            device_address (int): Address of the target device (1-based)
            command (str): SCPI command to send
            
        Returns:
            str: Response from the device, or None if error
        """
        if not self.connected:
            print("Error: Not connected to passthrough board")
            return None
        
        if device_address < 1 or device_address > self.device_count:
            print(f"Error: Invalid device address {device_address}. Valid range: 1-{self.device_count}")
            return None
        
        try:
            # Format the command with SCPI addressing
            addressed_command = f"{command} (@{device_address})"
            
            # Send the query through the passthrough board
            response = self.amu.query(addressed_command)
            
            return response.strip() if response else None
            
        except Exception as e:
            print(f"Error querying device {device_address}: {e}")
            return None
    
    def send_device(self, device_address, command):
        """
        Send a command to a specific device through the passthrough board
        
        Args:
            device_address (int): Address of the target device (1-based)
            command (str): SCPI command to send
            
        Returns:
            bool: True if command sent successfully, False otherwise
        """
        if not self.connected:
            print("Error: Not connected to passthrough board")
            return False
        
        if device_address < 1 or device_address > self.device_count:
            print(f"Error: Invalid device address {device_address}. Valid range: 1-{self.device_count}")
            return False
        
        try:
            # Format the command with SCPI addressing
            addressed_command = f"{command} (@{device_address})"
            
            # Send the command through the passthrough board
            self.amu.send(addressed_command)
            
            return True
            
        except Exception as e:
            print(f"Error sending command to device {device_address}: {e}")
            return False
    
    def query_all_devices(self, command):
        """
        Send a query to all connected devices
        
        Args:
            command (str): SCPI command to send to all devices
            
        Returns:
            dict: Dictionary mapping device address to response
        """
        if not self.connected:
            print("Error: Not connected to passthrough board")
            return {}
        
        responses = {}
        
        for device_addr in range(1, self.device_count + 1):
            response = self.query_device(device_addr, command)
            if response is not None:
                responses[device_addr] = response
            else:
                responses[device_addr] = "ERROR"
        
        return responses
    
    def get_device_info(self, device_address):
        """
        Get basic information about a specific device
        
        Args:
            device_address (int): Address of the target device
            
        Returns:
            dict: Dictionary containing device information
        """
        if not self.connected:
            print("Error: Not connected to passthrough board")
            return {}
        
        info = {}
        
        try:
            # Get basic device information
            commands = {
                'serial': 'SYST:SER?',
                'firmware': 'SYST:FIRM?',
                'hardware': 'SYST:HARD?',
                'temperature': 'SYST:TEMP?'
            }
            
            for key, command in commands.items():
                response = self.query_device(device_address, command)
                if response:
                    info[key] = response
                else:
                    info[key] = "N/A"
            
            return info
            
        except Exception as e:
            print(f"Error getting device info for device {device_address}: {e}")
            return {}
    
    def scan_all_devices(self):
        """
        Scan all connected devices and return their basic information
        
        Returns:
            dict: Dictionary mapping device address to device info
        """
        if not self.connected:
            print("Error: Not connected to passthrough board")
            return {}
        
        print(f"Scanning {self.device_count} connected devices...")
        
        all_devices = {}
        
        for device_addr in range(1, self.device_count + 1):
            print(f"  Scanning device {device_addr}...")
            device_info = self.get_device_info(device_addr)
            all_devices[device_addr] = device_info
        
        return all_devices


def main():
    """Main function demonstrating the passthrough interface"""
    parser = argparse.ArgumentParser(description='AMU TWI Passthrough Interface')
    parser.add_argument('--port', type=str,
                       help='Specify COM port (e.g., COM4). If not specified, auto-detect.')
    parser.add_argument('--scan', action='store_true',
                       help='Scan all connected devices and show their information')
    parser.add_argument('--device', type=int,
                       help='Query specific device address (1-based)')
    parser.add_argument('--command', type=str, default='SYST:SER?',
                       help='SCPI command to send (default: SYST:SER?)')
    
    args = parser.parse_args()
    
    # Create passthrough interface
    passthrough = AMUPassthrough(com_port=args.port)
    
    try:
        # Connect to the passthrough board
        if not passthrough.connect():
            print("Failed to connect to passthrough board")
            return 1
        else:
            # Clear existing errors and flush buffers with *CLS
            passthrough.amu.send("*CLS")
        
        if args.scan:
            # Scan all devices
            devices = passthrough.scan_all_devices()
            
            print("\n=== Device Scan Results ===")
            for addr, info in devices.items():
                print(f"\nDevice {addr}:")
                for key, value in info.items():
                    print(f"  {key.capitalize()}: {value}")
        
        elif args.device:
            # Query specific device
            if args.device > passthrough.get_device_count():
                print(f"Error: Device {args.device} not found. Available devices: 1-{passthrough.get_device_count()}")
                return 1
            
            print(f"Querying device {args.device} with command: {args.command}")
            response = passthrough.query_device(args.device, args.command)
            
            if response:
                print(f"Response: {response}")
            else:
                print("No response or error occurred")
        
        else:
            # Default: show device count and basic info
            device_count = passthrough.get_device_count()
            print(f"\nConnected to passthrough board with {device_count} remote devices")
            
            if device_count > 0:
                print("\nQuick device check (serial numbers):")
                serials = passthrough.query_all_devices("SYST:SER?")
                for addr, serial in serials.items():
                    print(f"  Device {addr}: {serial}")
                
                print(f"\nUse --scan to get detailed information about all devices")
                print(f"Use --device N --command 'COMMAND' to query a specific device")
        
    except KeyboardInterrupt:
        print("\nInterrupted by user")
    except Exception as e:
        print(f"Error: {e}")
        return 1
    finally:
        # Always disconnect
        passthrough.disconnect()
    
    return 0


if __name__ == "__main__":
    sys.exit(main())