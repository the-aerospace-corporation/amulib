import serial
import serial.tools.list_ports
import os
import sys
import time

class amu:

    serial_dev = None
    available = False


    def __init__(self):

        self.connect()

    

    def connect(self):

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
                    else:
                        print("Unable to open " + port.device + ". Close other applications with open ports.")
                        self.available = False
                    
                    return

                if port.vid == 0x303A and port.pid == 0x1001:
                    print("AMU 3.2 Found on " + port.device)

                    self.serial_dev = serial.Serial(port.device, 115200, timeout=3)

                    if self.serial_dev.isOpen():
                        print("AMU Communication started on " + self.serial_dev.name)
                        self.available = True
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

    def disconnect(self):
        if self.available == True:
            self.serial_dev.close()
            self.available = False
            print("Disonnecting AMU on " + self.serial_dev.name)

    
    def query(self, command):
        self.__write__(command)
        return self.serial_dev.readline().decode("utf-8").strip().strip('\"')
    
    def send(self, command, error_check=True):
        self.__write__(command)
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
            time.sleep(0.005)        # 5ms wait between sending next command

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


