import serial
import serial.tools.list_ports
import os
import sys
import time

class amu:

    serial_dev = None
    available = False


    def __init__(self, com_port=None):
        """
        Initialize AMU connection
        
        Args:
            com_port (str, optional): Specific COM port to connect to (e.g., 'COM4').
                                    If None, will auto-detect AMU device.
        """
        self.connect(com_port)

    

    def connect(self, com_port=None):
        """
        Connect to AMU device
        
        Args:
            com_port (str, optional): Specific COM port to connect to (e.g., 'COM4').
                                    If None, will auto-detect AMU device.
        """
        
        # If specific COM port is provided, try to connect directly
        if com_port:
            try:
                print(f"Attempting to connect to specified port: {com_port}")
                self.serial_dev = serial.Serial(com_port, 115200, timeout=3)
                
                if self.serial_dev.isOpen():
                    self.available = True

                    self.serial_dev.read_all()
                    self.serial_dev.flush()

                    self._clear_startup_messages()
                    
                    # Try to query hardware version to verify it's an AMU
                    try:
                        hardware_response = self.query("SYST:HARD?")
                        # Handle potential echo or formatting issues
                        if hardware_response and hardware_response.strip():
                            # Try to extract numeric value
                            import re
                            numeric_match = re.search(r'(\d+)', hardware_response)
                            if numeric_match:
                                hardwareVersion = int(numeric_match.group(1), 16)  # Assuming hex format
                                if hardwareVersion == 0x32:    
                                    print("AMU-ESP32 - 3.2 Communication started on " + self.serial_dev.name)
                                elif hardwareVersion == 0x33:    
                                    print("AMU-ESP32 - 3.3 Communication started on " + self.serial_dev.name)
                                else:
                                    print(f"AMU device found on {self.serial_dev.name} (Hardware version: 0x{hardwareVersion:02X})")
                            else:
                                print(f"AMU device found on {self.serial_dev.name} (Hardware response: {hardware_response})")
                        else:
                            print(f"AMU device found on {self.serial_dev.name} (Empty hardware response)")
                    except Exception as hw_error:
                        # If hardware query fails, check if it responds to basic commands
                        try:
                            response = self.query("*IDN?")
                            if "AMU" in response:
                                print(f"AMU device found on {self.serial_dev.name}")
                            else:
                                print(f"Connected to {self.serial_dev.name}, but device identification unclear")
                        except:
                            print(f"Connected to {self.serial_dev.name}, but unable to verify AMU device")
                    
                    # Clear startup messages
                    self._clear_startup_messages()
                    return
                else:
                    print(f"Unable to open {com_port}. Close other applications with open ports.")
                    self.available = False
                    return
                    
            except Exception as e:
                print(f"Failed to connect to {com_port}: {e}")
                print("Falling back to auto-detection...")
                # Fall through to auto-detection
        
        # Original auto-detection logic
        retries = 0
        amu_port = None

        while retries < 5:

            portList = serial.tools.list_ports.comports()

            for port in portList:
                #print(port.device, port.name, port.description, port.hwid, port.vid, port.pid, port.serial_number, port.manufacturer, port.product)
                print(port.device, port.vid, port.pid)

                if port.vid == 4617 and (port.pid == 257 or port.pid == 259):

                    if(port.pid == 257):
                        print("AMU Found on " + port.device)
                    else:
                        print("AMU 3 Found on " + port.device)

                    self.serial_dev = serial.Serial(port.device, 115200, timeout=3)
                    
                    if self.serial_dev.isOpen():
                        print("AMU Communication started on " + self.serial_dev.name)
                        self.available = True
                        # Clear startup messages

                        self.serial_dev.read_all()
                        self.serial_dev.flush()

                        self._clear_startup_messages()
                    else:
                        print("Unable to open " + port.device + ". Close other applications with open ports.")
                        self.available = False
                    
                    return

                if port.vid == 0x303A and port.pid == 0x1001:
                    

                    self.serial_dev = serial.Serial(port.device, 115200, timeout=3)

                    if self.serial_dev.isOpen():
                        self.available = True

                        # self.__write__("*RST")
                        
                        # Wait for device to finish reset and show "entry" confirmation
                        # self._wait_for_reset_complete()

                        # self._clear_startup_messages()

                        # Try to query hardware version to verify it's an AMU
                        try:
                            hardware_response = self.query("SYST:HARD?")
                            # Handle potential echo or formatting issues
                            if hardware_response and hardware_response.strip():
                                # Try to extract numeric value
                                import re
                                numeric_match = re.search(r'(\d+)', hardware_response)
                                if numeric_match:
                                    hardwareVersion = int(numeric_match.group(1), 16)  # Assuming hex format
                                    if hardwareVersion == 0x32:    
                                        print("AMU-ESP32 - 3.2 Communication started on " + self.serial_dev.name)
                                    elif hardwareVersion == 0x33:    
                                        print("AMU-ESP32 - 3.3 Communication started on " + self.serial_dev.name)
                                    else:
                                        print(f"AMU device found on {self.serial_dev.name} (Hardware version: 0x{hardwareVersion:02X})")
                                else:
                                    print(f"AMU device found on {self.serial_dev.name} (Hardware response: {hardware_response})")
                            else:
                                print(f"AMU device found on {self.serial_dev.name} (Empty hardware response)")
                        except Exception as hw_error:
                            print(f"AMU device found on {self.serial_dev.name} (Hardware query failed: {hw_error})")

                        # Clear startup messages
                        self._clear_startup_messages()
                    else:
                        print("Unable to open " + port.device + ". Close other applications with open ports.")
                        self.available = False
                    
                    return

            
            if(amu_port == None):
                time.sleep(1)
                retries = retries + 1
            else:
                retries = 100
        
        if amu_port == None:
            print("No AMU serial devices found.")

        #self.serial_dev = serial.Serial(serialPort, 115200, timeout=2)

    def _wait_for_reset_complete(self):
        """
        Wait for the device to complete reset and send the "entry" confirmation message.
        The device sends multiple lines after *RST, with the last line containing "entry".
        """
        if not self.available:
            return
        
        print("Waiting for device reset to complete...")
        timeout_start = time.time()
        timeout_duration = 10  # 10 second timeout
        
        while time.time() - timeout_start < timeout_duration:
            try:
                # Read line by line from the device
                if self.serial_dev.in_waiting > 0:
                    line = self.serial_dev.readline().decode("utf-8", errors='ignore').strip()
                    if line:
                        print(f"Reset output: '{line}'")  # Show exactly what we're receiving
                        
                        # Check for various possible completion indicators
                        line_lower = line.lower()
                        if any(keyword in line_lower for keyword in ["entry", "ready", "complete"]):
                            print("Device reset complete - ready for commands")
                            return
                        
                        # Check for the exact setup completion message (with potential trailing whitespace)
                        if "============ After Setup End =============" in line:
                            print("Device reset complete - ready for commands")
                            return
                            
                        # Check for setup end messages (more flexible matching)
                        if "setup end" in line_lower or "after setup" in line_lower:
                            print("Device reset complete - ready for commands")
                            return
                            
                        # Check for specific patterns that might indicate completion
                        if line.startswith("===") and "end" in line_lower:
                            print("Device reset complete - ready for commands") 
                            return
                
                time.sleep(0.1)  # Small delay to avoid busy waiting
                
            except Exception as e:
                print(f"Error while waiting for reset completion: {e}")
                break
        
        print("Warning: Timeout waiting for reset completion, proceeding anyway")

    def _clear_startup_messages(self):
        """
        Clear all startup messages by querying SYST:ERR? until we get "0,\"No error"
        """
        if not self.available:
            return
        
        # first clear everything from the read buffer
        self.serial_dev.read_all()

        max_attempts = 100  # Prevent infinite loop
        attempts = 0
        
        while attempts < max_attempts:
            try:
                error_response = self.query("SYST:ERR?")
                if error_response == "0,\"No error":
                    break
                elif error_response:
                    print(f"Clearing startup message: {error_response}")
                attempts += 1
                time.sleep(0.05)  # Small delay to avoid overwhelming the device
            except Exception as e:
                print(f"Error while clearing startup messages: {e}")
                break
        
        if attempts >= max_attempts:
            print("Warning: Maximum attempts reached while clearing startup messages")

    def disconnect(self):
        if self.available == True:
            self.serial_dev.close()
            self.available = False
            print("Disonnecting AMU on " + self.serial_dev.name)

    
    def query(self, command):
        self.__write__(command)
        
        # Small delay to let device start processing the command
        time.sleep(0.05)  # 5ms delay before reading response
        
        # Read the first response (readline() has built-in timeout)
        response = self.serial_dev.readline().decode("utf-8").strip().strip('\"')
        
        # If the response contains the command (echo), try to extract the actual response
        if command.replace('?', '') in response:
            # Look for a pattern like "COMMAND? (@1)" and try reading another line
            if '(@' in response:
                response = self.serial_dev.readline().decode("utf-8").strip().strip('\"')
            # If response equals command exactly, read next line
            elif response == command:
                response = self.serial_dev.readline().decode("utf-8").strip().strip('\"')
        
        return response
    
    def send(self, command, error_check=False):
        self.__write__(command)

        if( command == "*CLS" ):
            self.serial_dev.read_all()
            return 0
        
        if(error_check):
            error = ""
            while(error != "0,\"No error"):
                error = self.query("SYST:ERR?")
                if(error != "0,\"No error"):
                    print("AMU COMMAND \"" + command + "\" error: ", error)
                return error
            else:
                return 0
        else:
            return 0

    def __write__(self, command):
        if self.available == True:
            self.serial_dev.write(bytes((command + '\n'), "utf-8"))
            time.sleep(0.01)        # 10ms wait between sending next command

    def setCommTimeout(self, timeout=5):
        if self.available == True:
            self.serial_dev.timeout = timeout

    # deprecated flash function for atxmega128a4u based AMU versions
    # def flash(self):

    #     if self.available == True:

    #         self.send("SYST:BOOT")
    #         self.disconnect()

    #         time.sleep(2)

    #         os.system('dfu-programmer.exe atxmega128a4u flash --force ../firmware/versions/amu.hex')
    #         os.system('dfu-programmer.exe atxmega128a4u launch')

    #         time.sleep(1)

    #         self.connect()


