from amu_passthrough import AMUPassthrough


class TVACController:
    """
    TVAC (Thermal Vacuum) controller for managing multiple AMU devices
    through a passthrough board with persistent connection.
    """
    
    def __init__(self, com_port=None):
        """
        Initialize TVAC controller
        
        Args:
            com_port (str, optional): Specific COM port for passthrough board
        """
        self.passthrough = AMUPassthrough(com_port)
        self.connected = False
    
    def connect(self):
        """Connect to the passthrough board"""
        if not self.connected:
            self.connected = self.passthrough.connect()
        return self.connected
    
    def disconnect(self):
        """Disconnect from the passthrough board"""
        if self.connected:
            self.passthrough.disconnect()
            self.connected = False
    
    def __enter__(self):
        """Context manager entry - connect automatically"""
        if self.connect():
            return self
        else:
            raise RuntimeError("Failed to connect to passthrough board")
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit - disconnect automatically"""
        self.disconnect()

    def read_rtds(self):
        """Read RTD temperature sensors from all devices"""
        if not self.connected:
            raise RuntimeError("Not connected to passthrough board")
        
        rtd = self.passthrough.query_all_devices("MEAS:ADC:TSENS?")
        
        print("RTD readings from all devices:")
        for device_addr, temp in rtd.items():
            print(f"  Device {device_addr}: {temp}°C")
        
        return rtd

    def read_system_temperatures(self):
        """Read system temperatures from all devices"""
        if not self.connected:
            raise RuntimeError("Not connected to passthrough board")
        
        temperatures = self.passthrough.query_all_devices("SYST:TEMP?")
        
        print("Temperature readings from all devices:")
        for device_addr, temp in temperatures.items():
            print(f"  Device {device_addr}: {temp}°C")
        
        return temperatures
    
    def read_device_rtd(self, device_address):
        """
        Read RTD from a specific device
        
        Args:
            device_address (int): Device address (1-based)
            
        Returns:
            str: Temperature reading or None if error
        """
        if not self.connected:
            raise RuntimeError("Not connected to passthrough board")
        
        return self.passthrough.query_device(device_address, "MEAS:ADC:TSENS?")
    
    def read_device_system_temp(self, device_address):
        """
        Read system temperature from a specific device
        
        Args:
            device_address (int): Device address (1-based)
            
        Returns:
            str: Temperature reading or None if error
        """
        if not self.connected:
            raise RuntimeError("Not connected to passthrough board")
        
        return self.passthrough.query_device(device_address, "SYST:TEMP?")


# Legacy functions for backward compatibility
def read_rtds():
    """Legacy function - connects/disconnects for each call"""
    passthrough = AMUPassthrough()
    
    try:
        if passthrough.connect():
            rtd = passthrough.query_all_devices("MEAS:ADC:TSENS?")
            print("RTD readings from all devices:")
            for device_addr, temp in rtd.items():
                print(f"  Device {device_addr}: {temp}°C")
    finally:
        passthrough.disconnect()

def read_system_temperatures():
    """Legacy function - connects/disconnects for each call"""
    passthrough = AMUPassthrough()
    
    try:
        if passthrough.connect():
            temperatures = passthrough.query_all_devices("SYST:TEMP?")
            print("Temperature readings from all devices:")
            for device_addr, temp in temperatures.items():
                print(f"  Device {device_addr}: {temp}°C")
    finally:
        passthrough.disconnect()


def main():
    print("=== New Persistent Connection Method ===")
    # Method 1: Using context manager (recommended)
    try:
        with TVACController() as tvac:
            print("\nReading RTDs...")
            tvac.read_rtds()
            
            print("\nReading system temperatures...")
            tvac.read_system_temperatures()
            
            print("\nReading specific device RTD...")
            temp = tvac.read_device_rtd(1)
            print(f"Device 1 RTD: {temp}°C")
    
    except Exception as e:
        print(f"Error with persistent connection: {e}")
    
    print("\n=== Manual Connection Management ===")
    # Method 2: Manual connection management
    try:
        tvac = TVACController()
        if tvac.connect():
            print("Connected successfully")
            
            print("\nReading RTDs...")
            tvac.read_rtds()
            
            print("\nReading system temperatures...")
            tvac.read_system_temperatures()
            
            tvac.disconnect()
        else:
            print("Failed to connect")
    
    except Exception as e:
        print(f"Error with manual connection: {e}")
    
    print("\n=== Legacy Method (Connect/Disconnect Each Call) ===")
    try:
        read_rtds()
        read_system_temperatures()
        
    except KeyboardInterrupt:
        print("\nExamples interrupted by user")
    except Exception as e:
        print(f"Error in legacy examples: {e}")


if __name__ == "__main__":
    main()