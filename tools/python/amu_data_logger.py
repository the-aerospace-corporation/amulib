"""
AMU Data Logger

A persistent connection class for long-term data acquisition from multiple AMU devices
through a passthrough board. Designed to be triggered by external programs for 
measurements at different intervals without connection overhead.

Author: AMU Library
Date: October 2025
"""

from amu_passthrough import AMUPassthrough
import time
import threading
from datetime import datetime
import json


class AMUDataLogger:
    """
    Persistent AMU data logger for long-term measurements
    
    This class maintains a persistent connection to the AMU passthrough board
    and allows for repeated queries and commands without reconnecting.
    Designed for external program triggers and continuous monitoring.
    """
    
    def __init__(self, com_port=None, auto_connect=True):
        """
        Initialize the AMU Data Logger
        
        Args:
            com_port (str, optional): Specific COM port for passthrough board
            auto_connect (bool): Automatically connect on initialization
        """
        self.passthrough = AMUPassthrough(com_port)
        self.connected = False
        self.device_count = 0
        self.last_error = None
        self._lock = threading.Lock()  # Thread safety for concurrent access
        
        if auto_connect:
            self.connect()
    
    def connect(self):
        """
        Connect to the passthrough board and discover devices
        
        Returns:
            bool: True if connected successfully, False otherwise
        """
        with self._lock:
            if not self.connected:
                try:
                    self.connected = self.passthrough.connect()
                    if self.connected:
                        self.device_count = self.passthrough.device_count
                        print(f"Connected to passthrough board with {self.device_count} devices")
                        return True
                    else:
                        self.last_error = "Failed to connect to passthrough board"
                        return False
                except Exception as e:
                    self.last_error = f"Connection error: {e}"
                    print(f"Error connecting: {e}")
                    return False
            return True
    
    def disconnect(self):
        """Disconnect from the passthrough board"""
        with self._lock:
            if self.connected:
                try:
                    self.passthrough.disconnect()
                    self.connected = False
                    print("Disconnected from passthrough board")
                except Exception as e:
                    print(f"Error during disconnect: {e}")
    
    def is_connected(self):
        """Check if currently connected"""
        return self.connected
    
    def get_device_count(self):
        """Get number of connected devices"""
        return self.device_count if self.connected else 0
    
    def query_device(self, device_address, command):
        """
        Send a query to a specific device
        
        Args:
            device_address (int): Device address (1-based)
            command (str): SCPI command to send
            
        Returns:
            str: Response from device or None if error
        """
        if not self.connected:
            self.last_error = "Not connected to passthrough board"
            return None
        
        try:
            with self._lock:
                response = self.passthrough.query_device(device_address, command)
                return response
        except Exception as e:
            self.last_error = f"Query error for device {device_address}: {e}"
            print(f"Error querying device {device_address}: {e}")
            return None
    
    def query_all_devices(self, command):
        """
        Send a query to all connected devices
        
        Args:
            command (str): SCPI command to send to all devices
            
        Returns:
            dict: Dictionary mapping device address to response
        """
        if not self.connected:
            self.last_error = "Not connected to passthrough board"
            return {}
        
        try:
            with self._lock:
                responses = self.passthrough.query_all_devices(command)
                return responses
        except Exception as e:
            self.last_error = f"Query all error: {e}"
            print(f"Error querying all devices: {e}")
            return {}
    
    def send_device(self, device_address, command):
        """
        Send a command to a specific device
        
        Args:
            device_address (int): Device address (1-based)
            command (str): SCPI command to send
            
        Returns:
            bool: True if command sent successfully, False otherwise
        """
        if not self.connected:
            self.last_error = "Not connected to passthrough board"
            return False
        
        try:
            with self._lock:
                success = self.passthrough.send_device(device_address, command)
                return success
        except Exception as e:
            self.last_error = f"Send error for device {device_address}: {e}"
            print(f"Error sending to device {device_address}: {e}")
            return False
    
    def get_last_error(self):
        """Get the last error message"""
        return self.last_error
    
    # Convenience methods for common measurements
    def read_all_rtd_temperatures(self):
        """
        Read RTD temperatures from all devices
        
        Returns:
            dict: Dictionary mapping device address to temperature
        """
        return self.query_all_devices("MEAS:ADC:TSENS?")
    
    def read_all_system_temperatures(self):
        """
        Read system temperatures from all devices
        
        Returns:
            dict: Dictionary mapping device address to temperature
        """
        return self.query_all_devices("SYST:TEMP?")
    
    def read_all_voltages(self):
        """
        Read voltages from all devices
        
        Returns:
            dict: Dictionary mapping device address to voltage
        """
        return self.query_all_devices("MEAS:VOLT?")
    
    def read_all_currents(self):
        """
        Read currents from all devices
        
        Returns:
            dict: Dictionary mapping device address to current
        """
        return self.query_all_devices("MEAS:CURR?")
    
    def read_device_rtd_temperature(self, device_address):
        """Read RTD temperature from specific device"""
        return self.query_device(device_address, "MEAS:ADC:TSENS?")
    
    def read_device_system_temperature(self, device_address):
        """Read system temperature from specific device"""
        return self.query_device(device_address, "SYST:TEMP?")
    
    def read_device_voltage(self, device_address):
        """Read voltage from specific device"""
        return self.query_device(device_address, "MEAS:VOLT?")
    
    def read_device_current(self, device_address):
        """Read current from specific device"""
        return self.query_device(device_address, "MEAS:CURR?")
    
    def get_all_device_info(self):
        """
        Get identification information from all devices
        
        Returns:
            dict: Dictionary mapping device address to device info
        """
        return self.query_all_devices("*IDN?")
    
    def get_device_info(self, device_address):
        """Get identification information from specific device"""
        return self.query_device(device_address, "*IDN?")
    
    def take_snapshot(self, include_temps=True, include_electrical=True):
        """
        Take a complete snapshot of all measurements from all devices
        
        Args:
            include_temps (bool): Include temperature measurements
            include_electrical (bool): Include voltage/current measurements
            
        Returns:
            dict: Complete measurement snapshot with timestamp
        """
        snapshot = {
            'timestamp': datetime.now().isoformat(),
            'device_count': self.device_count,
            'measurements': {}
        }
        
        if include_temps:
            snapshot['measurements']['rtd_temperatures'] = self.read_all_rtd_temperatures()
            snapshot['measurements']['system_temperatures'] = self.read_all_system_temperatures()
        
        if include_electrical:
            snapshot['measurements']['voltages'] = self.read_all_voltages()
            snapshot['measurements']['currents'] = self.read_all_currents()
        
        return snapshot
    
    def __enter__(self):
        """Context manager entry"""
        if self.connect():
            return self
        else:
            raise RuntimeError(f"Failed to connect: {self.last_error}")
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit"""
        self.disconnect()
    
    def __del__(self):
        """Destructor - ensure disconnection"""
        if hasattr(self, 'connected') and self.connected:
            self.disconnect()


def main():
    """Example usage of AMUDataLogger"""
    
    print("=== AMU Data Logger Example ===")
    
    # Example 1: Context manager (auto-disconnect)
    print("\n1. Context manager usage:")
    try:
        with AMUDataLogger() as logger:
            print(f"Connected with {logger.get_device_count()} devices")
            
            # Take a few measurements
            rtd_temps = logger.read_all_rtd_temperatures()
            print(f"RTD Temperatures: {rtd_temps}")
            
            # Take a complete snapshot
            snapshot = logger.take_snapshot()
            print(f"Snapshot taken at: {snapshot['timestamp']}")
            
    except Exception as e:
        print(f"Error: {e}")
    
    # Example 2: Manual management (persistent connection)
    print("\n2. Manual connection management:")
    logger = AMUDataLogger(auto_connect=False)
    
    if logger.connect():
        print("Connected successfully")
        
        # Simulate multiple measurement cycles
        for cycle in range(3):
            print(f"\nMeasurement cycle {cycle + 1}:")
            
            # Read from device 1
            temp = logger.read_device_rtd_temperature(1)
            voltage = logger.read_device_voltage(1)
            print(f"  Device 1 - Temp: {temp}°C, Voltage: {voltage}V")
            
            # Read from all devices
            all_temps = logger.read_all_system_temperatures()
            print(f"  All system temps: {all_temps}")
            
            time.sleep(1)  # Simulate time between measurements
        
        logger.disconnect()
    else:
        print(f"Failed to connect: {logger.get_last_error()}")


if __name__ == "__main__":
    main()