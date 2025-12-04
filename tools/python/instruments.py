import pyvisa
import time

rm = pyvisa.ResourceManager()


def list_visa_resources():
    """
    List all available PyVISA resources.
    
    Returns:
        list: A list of VISA resource strings for all available instruments
    """
    try:
        resources = rm.list_resources()
        if resources:
            print("Available VISA Resources:")
            for i, resource in enumerate(resources, 1):
                print(f"  {i}. {resource}")
        else:
            print("No VISA resources found.")
        return list(resources)
    except Exception as e:
        print(f"Error listing VISA resources: {e}")
        return []


class instrument:

    visa_id = None
    dev = None

    def __init__(self, visa_id):
        self.visa_id = visa_id
        self.dev = rm.open_resource(self.visa_id)
        print(self.dev)
    
    def write(self, command):
        self.dev.write(command)
        time.sleep(0.2)

    def query(self, command):
        return self.dev.query(command)

    def close(self):
        self.dev.close()

class CalibrationInstruments:
    """
    A class for managing calibration instruments including source meter and optional voltage/current meters.
    
    Attributes:
        source_meter: The source meter instrument (required)
        voltage_meter: The voltage meter instrument (optional)
        current_meter: The current meter instrument (optional)
    """
    
    def __init__(self, source_meter_visa, volt_meter_visa=None, current_meter_visa=None):
        """
        Initialize the calibration instruments.
        
        Args:
            source_meter_visa (str): VISA address for the source meter (required)
            volt_meter_visa (str, optional): VISA address for the voltage meter
            current_meter_visa (str, optional): VISA address for the current meter
        """
        self.source_meter = None
        self.voltage_meter = None
        self.current_meter = None
        
        # Initialize source meter (required)
        self.source_meter = instrument(source_meter_visa)
        
        # Initialize optional meters
        if volt_meter_visa is not None:
            self.voltage_meter = instrument(volt_meter_visa)
        
        if current_meter_visa is not None:
            self.current_meter = instrument(current_meter_visa)
        
        # Configure instruments
        self._configure_instruments()
    
    def _configure_instruments(self):
        """Configure all connected instruments with default settings."""
        # Configure source meter
        self.source_meter.write("*CLS")                 # clear status
        self.source_meter.write(":SENS:REM ON")         # set remote sensing on
        self.source_meter.write(":OUTP:LOW FLO")        # set output to floating if possible

        # Configure voltage meter if available
        if self.voltage_meter is not None:
            self.voltage_meter.write("*CLS")
            self.voltage_meter.write("CONF:VOLT:DC AUTO,MIN")

        # Configure current meter if available
        if self.current_meter is not None:
            self.current_meter.write("*CLS")
            self.current_meter.write("CONF:CURR:DC AUTO,MIN")

    def close_instruments(self):
        """Safely close all connected instruments."""
        self.source_meter.write(":OUTP OFF")
        time.sleep(1)
        
        if self.source_meter is not None:
            self.source_meter.close()
        
        if self.voltage_meter is not None:
            self.voltage_meter.close()
        
        if self.current_meter is not None:
            self.current_meter.close()

    def set_voltage_mode(self, vmax, res="MIN", current_limit=0.050):
        """
        Configure instruments for voltage sourcing mode.
        
        Args:
            vmax (float): Maximum voltage to be measured
            res (str): Resolution setting for voltage meter
            current_limit (float): Current protection limit
        """

        range_max = ".01"
        if vmax > .01:
            range_max = ".1"
        if vmax > .1:
            range_max = "1"
        if vmax > 1:
            range_max = "10"
        if vmax > 10:
            range_max = "100"

        # Configure voltage meter range if available
        if self.voltage_meter is not None:
            self.voltage_meter.write("CONF:VOLT:DC " + range_max + "," + str(res))

        # Configure source meter for voltage mode
        self.source_meter.write(":SOUR:FUNC:MODE VOLT")                      # set source mode to voltage output
        self.source_meter.write(":SENS:CURR:PROT " + str(current_limit))     # set current protection to current_limit
        self.source_meter.write(":SOUR:VOLT:RANG:AUTO OFF")                  # set voltage range to manual
        self.source_meter.write(":SOUR:VOLT:RANG " + str(range_max))         # set voltage range upper limit
        self.source_meter.write(":SOUR:VOLT 0.000")                          # set output voltage to 0V
        self.source_meter.write(":OUTP ON")                                  # enable output

    def set_current_mode(self, imax, res="MIN", voltage_limit=2.500):
        """
        Configure instruments for current sourcing mode.
        
        Args:
            imax (float): Maximum current to be measured
            res (str): Resolution setting for current meter
            voltage_limit (float): Voltage protection limit
        """
        
        range_max = ".01"
        if imax > .01:
            range_max = ".1"
        if imax > .1:
            range_max = "1"
        if imax > 1:
            range_max = "3"

        # Configure current meter range if available
        if self.current_meter is not None:
            self.current_meter.write("CONF:CURR:DC " + range_max + "," + str(res))

        # Configure source meter for current mode
        self.source_meter.write(":SOUR:FUNC:MODE CURR")                         # set source mode to current output
        self.source_meter.write(":SENS:VOLT:PROT " + str(voltage_limit))        # set voltage protection to voltage_limit
        self.source_meter.write(":SOUR:CURR:RANG:AUTO OFF")                     # set current range to manual
        self.source_meter.write(":SOUR:CURR:RANG " + str(range_max))            # set current range upper limit
        self.source_meter.write(":SOUR:CURR 0.000")                             # set output current to 0A
        self.source_meter.write(":OUTP ON")                                     # enable output

    def set_voltage(self, voltage):
        """
        Set the source meter output voltage.
        
        Args:
            voltage (float): Voltage to set
        """
        self.source_meter.write(":SOUR:VOLT " + str(voltage))                # set output voltage to voltage

    def set_current(self, current):
        """
        Set the source meter output current.
        
        Args:
            current (float): Current to set
        """
        self.source_meter.write(":SOUR:CURR " + str(current))                # set output current to current

    def measure_voltage(self):
        """
        Measure voltage using the voltage meter.
        
        Returns:
            float: Measured voltage, or 0 if no voltage meter is connected
        """
        if self.voltage_meter is None:
            return float(self.source_meter.query("MEAS:VOLT?").strip())
        else:
            return float(self.voltage_meter.query("READ?").strip())

    def measure_current(self):
        """
        Measure current using the current meter.
        
        Returns:
            float: Measured current, or 0 if no current meter is connected
        """
        if self.current_meter is None:
            return float(self.source_meter.query("MEAS:CURR?").strip())
        else:
            return float(self.current_meter.query("READ?").strip())

    def set_voltage_with_feedback(self, value, max_error=1e-6, current_limit=0.250, retries=5):
        """
        Set voltage with feedback control using the voltage meter.
        
        Args:
            value (float): Target voltage value
            max_error (float): Maximum allowable error
            current_limit (float): Current limit for protection
            retries (int): Maximum number of retry attempts
        """
        if self.voltage_meter is None:
            # print("Warning: No voltage meter connected. Using source meter without feedback.")
            self.set_voltage(value)
            return

        delta = 0
        iterator = 1

        print("Attempt", "Set\t", "Meter", "Delta", sep='\t')

        setpoint = value

        while retries > 0:
            # set source value
            setpoint -= delta
            
            self.source_meter.write(":SOUR:VOLT " + str(setpoint))              # set source to value

            time.sleep(0.25)

            # measure value
            measurement = self.measure_voltage()
            
            delta = measurement - value
            
            print("{}\t{:<06e}\t{:<06e}\t{:<06e}".format(iterator, setpoint, measurement, delta))

            if abs(delta) < max_error:
                return
            else:
                retries = retries - 1
                iterator += 1

    def set_current_with_feedback(self, value, max_error=1e-6, retries=5):
        """
        Set current with feedback control using the current meter.
        
        Args:
            value (float): Target current value
            max_error (float): Maximum allowable error
            retries (int): Maximum number of retry attempts
        """
        if self.current_meter is None:
            # print("Warning: No current meter connected. Using source meter without feedback.")
            self.set_current(value)
            return

        delta = 0
        iterator = 1

        print("Attempt", "Set\t", "Meter", "Delta", sep='\t')

        setpoint = value

        while retries > 0:
            # set source value
            setpoint -= delta
            
            self.source_meter.write(":SOUR:CURR " + str(setpoint))              # set source to value

            time.sleep(0.25)

            # measure value
            measurement = self.measure_current()
            
            delta = measurement - value

            print("{}\t{:<06e}\t{:<06e}\t{:<06e}".format(iterator, setpoint, measurement, delta))

            if abs(delta) < max_error:
                return
            else:
                retries = retries - 1
                iterator += 1
# Example usage
if __name__ == "__main__":
    print("=== PyVISA Instrument Control Examples ===\n")
    
    # Example 1: List available VISA resources
    print("1. Listing available VISA resources:")
    available_resources = list_visa_resources()
    print()
    
    if not available_resources:
        print("No instruments found. Make sure instruments are connected and drivers are installed.")
        exit()
    
    # Example 2: Connect to a single instrument
    print("2. Connecting to a single instrument:")
    try:
        # Use the first available resource as an example
        print(f"Connecting to: {available_resources[0]}")
        my_instrument = instrument(available_resources[0])
        
        # Query instrument identification
        idn = my_instrument.query("*IDN?")
        print(f"Instrument ID: {idn.strip()}")
        
        # Close the connection
        my_instrument.close()
        print("Single instrument connection closed.\n")
        
    except Exception as e:
        print(f"Error connecting to single instrument: {e}\n")
    
    # Example 3: Calibration setup (requires source meter)
    print("3. Calibration instruments setup example:")
    try:
        # Note: Modify these VISA addresses to match your actual instruments
        source_meter_visa = available_resources[0]  # Use first available as source meter
        
        # Optional: If you have multiple instruments, specify them here
        volt_meter_visa = available_resources[1] if len(available_resources) > 1 else None
        current_meter_visa = available_resources[2] if len(available_resources) > 2 else None
        
        print(f"Source meter: {source_meter_visa}")
        if volt_meter_visa:
            print(f"Voltage meter: {volt_meter_visa}")
        if current_meter_visa:
            print(f"Current meter: {current_meter_visa}")
        
        # Initialize calibration instruments
        cal_instruments = CalibrationInstruments(
            source_meter_visa=source_meter_visa,
            volt_meter_visa=volt_meter_visa,
            current_meter_visa=current_meter_visa
        )
        
        # Example voltage mode operation
        print("\nSetting up voltage mode (1V max, 50mA current limit):")
        cal_instruments.set_voltage_mode(vmax=1.0, current_limit=0.050)
        
        # Set a voltage and measure
        print("Setting voltage to 0.5V:")
        cal_instruments.set_voltage(0.5)
        time.sleep(0.5)  # Allow settling time
        
        measured_voltage = cal_instruments.measure_voltage()
        measured_current = cal_instruments.measure_current()
        print(f"Measured voltage: {measured_voltage:.6f} V")
        print(f"Measured current: {measured_current:.6f} A")
        
        # Example current mode operation
        print("\nSetting up current mode (10mA max, 2.5V voltage limit):")
        cal_instruments.set_current_mode(imax=0.01, voltage_limit=2.5)
        
        # Set a current and measure
        print("Setting current to 1mA:")
        cal_instruments.set_current(0.001)
        time.sleep(0.5)  # Allow settling time
        
        measured_voltage = cal_instruments.measure_voltage()
        measured_current = cal_instruments.measure_current()
        print(f"Measured voltage: {measured_voltage:.6f} V")
        print(f"Measured current: {measured_current:.6f} A")
        
        # Clean up
        cal_instruments.close_instruments()
        print("Calibration instruments closed safely.")
        
    except Exception as e:
        print(f"Error in calibration setup: {e}")
        print("This example requires at least one compatible source meter instrument.")
    
    print("\n=== Examples completed ===")
    print("\nTo use this module in your own code:")
    print("1. Import the module: from instruments import *")
    print("2. List resources: resources = list_visa_resources()")
    print("3. Connect to instrument: my_instr = instrument(resources[0])")
    print("4. Or use calibration setup: cal = CalibrationInstruments(visa_address)")
    print("\nRemember to call close() or close_instruments() when done!")
