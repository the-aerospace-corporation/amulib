import sys
import time
import argparse
import winsound
from datetime import datetime
from amu import amu as AMU
import numpy as np
import teleplot as teleplot
from instruments import CalibrationInstruments


source_meter_visa = 'USB0::0x0957::0x8D18::MY51140506::0::INSTR'
# volt_meter_visa = 'GPIB0::1::INSTR'
# current_meter_visa = 'GPIB0::2::INSTR'

# instruments = CalibrationInstruments(source_meter_visa, volt_meter_visa, current_meter_visa)
instruments = CalibrationInstruments(source_meter_visa)

amu = None


def filePrintln(file, text=""):
    if(file != None):
        file.write(text + "\n")
    print(text)

def info(file=None):

    hardwareVersion = int(amu.query("SYST:HARD?"))

    notes = amu.query("DUT:NOTES?")
    address = amu.query("SYST:TWI:ADD?")
    serial = amu.query("SYST:SER?")
    config = amu.query("SWEEP:CONFIG?")

    if hardwareVersion == 0x01:    print("isc² - 0.1")
    if hardwareVersion == 0x10:    print("AMU-128X - 1.0")
    if hardwareVersion == 0x11:    print("AMU-128X - 1.1")
    if hardwareVersion == 0x20:    print("AMU-128X - 2.0")
    if hardwareVersion == 0x21:    print("AMU-128X - 2.1")
    if hardwareVersion == 0x30:    print("AMU-128X - 3.0")
    if hardwareVersion == 0x31:    print("AMU-ESP32 - 3.1")
    if hardwareVersion == 0x32:    print("AMU-ESP32 - 3.2")
    if hardwareVersion == 0x33:    print("AMU-ESP32 - 3.3")

    filePrintln(file, "NOTES: " + notes)
    filePrintln(file, "MANUFACTURER: " + amu.query("DUT:MAN?"))
    filePrintln(file, "MODEL: " + amu.query("DUT:MOD?"))
    filePrintln(file, "TECH: " + amu.query("DUT:TECH?"))
    filePrintln(file, "SERIAL: " + amu.query("DUT:SER?"))
    filePrintln(file, "COVERGLASS: " + amu.query("DUT:COVER?"))
    filePrintln(file, "JUNCTION: " + amu.query("DUT:JUNC?"))
    filePrintln(file, "ENERGY: " + amu.query("DUT:ENERGY?"))
    filePrintln(file, "DOSE: " + amu.query("DUT:DOSE?"))

    filePrintln(file, "ADDRESS: " + address)
    filePrintln(file, "AMU-SERIAL: " + serial)
    filePrintln(file, "CONFIG: " + config)

    filePrintln(file, "DAC-GAIN-CORR: " + amu.query("DAC:GAIN:CORR?"))
    filePrintln(file, "DAC-OFFSET-CORR: " + amu.query("DAC:OFF:CORR?"))

    filePrintln(file, "ADC VOLTAGE PGA: " + amu.query("ADC:VOLT:PGA?") + " (Max: " + amu.query("ADC:VOLT:MAX?") + " V)")
    filePrintln(file, "ADC CURRENT PGA: " + amu.query("ADC:CURR:PGA?") + " (Max: " + amu.query("ADC:CURR:MAX?") + " A)")

def getIVCoefficients():

    voltagePGA = amu.query("ADC:VOLT:PGA?")
    currentPGA = amu.query("ADC:CURR:PGA?")

    coeff_str = "PGA\tVoff\tVgain\tIoff\tIgain\n"

    for i in range(8):
        amu.send("ADC:VOLT:PGA " + str(i))
        amu.send("ADC:CURR:PGA " + str(i))
        coeff_str += str(i) + "\t"
        coeff_str += amu.query("ADC:VOLT:OFF?") + "\t"
        coeff_str += amu.query("ADC:VOLT:GAIN?") + "\t"
        coeff_str += amu.query("ADC:CURR:OFF?") + "\t"
        coeff_str += amu.query("ADC:CURR:GAIN?") + "\n"
    
    amu.send("ADC:VOLT:PGA " + voltagePGA)      # reset original voltage PGA setting
    amu.send("ADC:CURR:PGA " + currentPGA)      # reset original current PGA setting

    return coeff_str


# def getCoefficients():

#     offset = []
#     gain = []
#     filter = []
#     yaw = []
#     pitch = []

#     print("CH#", "OFFSET", "\tGAIN", sep='\t')
#     for i in range(16):

#         offset.append(int(amu.query("ADC:CH" + str(i) + ":OFF?")))
#         gain.append(int(amu.query("ADC:CH" + str(i) + ":GAIN?")))

#         print("CH"+str(i), offset[i], gain[i], sep='\t')

#     yaw.append(amu.query("SS:FIT:YAW?"))
#     pitch.append(amu.query("SS:FIT:PITCH?"))

#     print("YAW", yaw, sep='\t')
#     print("PITCH", pitch, sep='\t')

    
def calibrateVoltage(pga = 0, steps = 10, save=False, printResult=False, file = None):

    filePrintln(file, "\n*** VOLTAGE CALIBRATION - PGA " + str(pga) + " ***\n")

    amu.query("SWEEP:TRIG:VOC?")      # make sure open circuit voltage condition is in place
    time.sleep(0.5)

    amu.send("ADC:VOLT:PGA " + str(pga))   # set voltage pga

    vmax = float(amu.query("ADC:VOLT:MAX?").strip())

    # filePrintln(file, "ADC:CH0:OFF?\t" + amu.query("ADC:CH0:OFF?"))
    # filePrintln(file, "ADC:CH0:GAIN?\t" + amu.query("ADC:CH0:GAIN?"))

    instruments.set_voltage_mode(vmax, res=.0001)
    
    # Set source meter to zero output voltage, adjust until meter reads <10uV
    instruments.set_voltage_with_feedback(0.000, max_error=vmax*.0001)
    # setVoltage(0.00)
    time.sleep(0.5)

    filePrintln(None, "\nVoltage Set to ZERO")
    filePrintln(None, "ADC:VOLT:OFF?\t" + amu.query("ADC:VOLT:OFF?"))
    filePrintln(None, "ADC:VOLT:GAIN?\t" + amu.query("ADC:VOLT:GAIN?"))

    # Perform AMU offset calibration
    amu.send("ADC:VOLT:CAL:ZERO", error_check=False)
    time.sleep(2.5)

    filePrintln(None, "\nCAL ZERO")
    filePrintln(file, "ADC:VOLT:OFF?\t" + amu.query("ADC:VOLT:OFF?"))
    filePrintln(file, "ADC:VOLT:GAIN?\t" + amu.query("ADC:VOLT:GAIN?"))

    # Set source meter to full scale voltage * 0.98 from vmax, adjust until meter reads <10uV
    instruments.set_voltage_with_feedback(vmax * 0.98, max_error=vmax*.000001)
    # setVoltage(vmax)

    filePrintln(None, "\nVoltage Set to " + str(vmax * 0.98) + " Volts")
    # filePrintln(file, "ADC:CH0:OFF?\t" + amu.query("ADC:CH0:OFF?"))
    # filePrintln(file, "ADC:CH0:GAIN?\t" + amu.query("ADC:CH0:GAIN?"))
    
    time.sleep(0.5)
    instruments.measure_voltage()
    
    # Perform AMU full scale calibration
    amu.send("ADC:VOLT:CAL:FULL", error_check=False)
    time.sleep(2.5)
    # amu.send("ADC:CH0:CAL:FULL")

    filePrintln(None, "\nCAL FULL")
    filePrintln(None, "ADC:VOLT:OFF?\t" + amu.query("ADC:VOLT:OFF?"))
    filePrintln(None, "ADC:VOLT:GAIN?\t" + amu.query("ADC:VOLT:GAIN?"))

    if save:
        amu.send("ADC:VOLT:CAL:SAVE")


def checkVoltageCal(steps=10, file=None):

    amu.query("SWEEP:TRIG:VOC?")      # make sure open circuit voltage condition is in place
    time.sleep(0.5)

    vmax = float(amu.query("ADC:VOLT:MAX?").strip())

    # if(vmax > 10):
    #     setVoltageMode(vmax, res=1e-5, current_limit = 0.005)
    # else:
    #     setVoltageMode(vmax, res=1e-6, current_limit = 0.005)
    instruments.set_voltage_mode(vmax, res='MIN', current_limit = 0.005)
        
    filePrintln(file, "Set(V)\tMeter(V)\tAMU(V)\t\tDelta(µV)\tDelta(%)\tDelta(ppm)")
    
    delta = []
    percent = []

    vstart = vmax * 0.98

    step = vstart / steps

    for i in range(steps + 1):
        voltage_setpoint = vstart - (step * i)
        instruments.set_voltage(voltage_setpoint)
        time.sleep(0.1)
        meterVoltage = instruments.measure_voltage()
        amuVoltage = float(amu.query("MEAS:ADC:VOLT?"))

        delta.append(abs(meterVoltage - amuVoltage))
        delta_uV = delta[i]*1000000
        delta_percent = abs(delta[i] / vmax * 100)

        percent.append(delta_percent)
        
        filePrintln(file, "{:<6.04f}\t{:<8.06f}\t{:<8.06f}\t{:<8.02f}\t{:<8.04}\t{:<8.02f}".format(voltage_setpoint, meterVoltage, amuVoltage, delta_uV, delta_percent, delta_percent * 10000))
    
    filePrintln(file, "\nVmax\t{:.04f}".format(vmax))
    filePrintln(file, "Average Delta (µV)\t{:.04f}".format(np.mean(delta)*1000000))
    filePrintln(file, "Average Error\t{:.04f}%\t{:.02f}ppm".format(np.mean(percent), np.mean(percent) * 10000))
        
def calibrateCurrent(pga = 0, steps = 10, save=False, printResult=False, file=None):

    filePrintln(file, "\n*** CURRENT CALIBRATION - PGA " + str(pga) + " ***\n")

    amu.query("SWEEP:TRIG:ISC?")      # make sure short circuit condition is in place
    time.sleep(0.50)

    amu.send("ADC:CURR:PGA " + str(pga))   # set current pga
    time.sleep(0.25)

    imax = float(amu.query("ADC:CURR:MAX?"))

    instruments.set_current_mode(imax, res='MIN')

    # Set source meter to zero output voltage, adjust until meter reads <10uV
    instruments.set_current_with_feedback(0.000, max_error=imax*.00001)

    filePrintln(None, "\nCurrent Set to ZERO")
    filePrintln(None, "ADC:CURR:OFF?\t" + amu.query("ADC:CURR:OFF?"))
    filePrintln(None, "ADC:CURR:GAIN?\t" + amu.query("ADC:CURR:GAIN?"))

    time.sleep(0.5)
    # Perform AMU offset calibration
    amu.send("ADC:CURR:CAL:ZERO", error_check=False)    
    time.sleep(2.5)

    filePrintln(None, "\nCAL ZERO")
    filePrintln(file, "ADC:CURR:OFF?\t" + amu.query("ADC:CURR:OFF?"))
    filePrintln(file, "ADC:CURR:GAIN?\t" + amu.query("ADC:CURR:GAIN?"))


    # Set source meter to full scale voltage from vmax, adjust until meter reads <10uV
    instruments.set_current_with_feedback(imax*0.98, max_error=imax*.00001)
    time.sleep(0.5)
    instruments.measure_current()

    filePrintln(None, "\nCurrent Set to " + str(imax*0.98) + " Amps")
    filePrintln(file, "ADC:CURR:OFF?\t" + amu.query("ADC:CURR:OFF?"))
    filePrintln(file, "ADC:CURR:GAIN?\t" + amu.query("ADC:CURR:GAIN?"))

    # Perform AMU full scale calibration
    amu.send("ADC:CURR:CAL:FULL", error_check=False)
    time.sleep(2.5)
    # amu.send("ADC:CH1:CAL:FULL")

    filePrintln(None, "\nCAL FULL")
    filePrintln(None, "ADC:CURR:OFF?\t" + amu.query("ADC:CURR:OFF?"))
    filePrintln(None, "ADC:CURR:GAIN?\t" + amu.query("ADC:CURR:GAIN?"))

    if save:
        amu.send("ADC:CURR:CAL:SAVE")

    
def checkCurrentCal(steps=10, file=None):

    amu.query("SWEEP:TRIG:ISC?")      # make sure short circuit condition is in place
    time.sleep(0.5)

    imax = float(amu.query("ADC:CURR:MAX?").strip())

    # Check calibration:
    # if(imax > 1):
    #     setCurrentMode(imax, res=1e-4)
    # else:
    #     setCurrentMode(imax, res=1e-5)

    instruments.set_current_mode(imax, res='MIN', voltage_limit=5.00)

    filePrintln(file, "Set(A)\tMeter(A)\tAMU(A)\t\tDelta(µA)\tDelta(%)\tDelta(ppm)")

    delta = []
    percent = []

    istart = imax * 0.98

    step = istart / steps

    for i in range(steps + 1):
        current_setpoint = istart - (step * i)
        instruments.set_current(current_setpoint)
        time.sleep(0.5)
        meterCurrent = instruments.measure_current()
        amuCurrent = float(amu.query("MEAS:ADC:CURR?"))

        delta.append(abs(meterCurrent - amuCurrent))
        delta_uA = delta[i]*1000000
        delta_percent = abs(delta[i] / imax * 100)

        percent.append(delta_percent)
        
        filePrintln(file, "{:<6.04f}\t{:<8.06f}\t{:<8.06f}\t{:<8.02f}\t{:<8.04}\t{:<8.02f}".format(current_setpoint, meterCurrent, amuCurrent, delta_uA, delta_percent, delta_percent * 10000))
    
    filePrintln(file, "\nImax\t{:.04f}".format(imax))
    filePrintln(file, "Average Delta (µA)\t{:.04f}".format(np.mean(delta)*1000000))
    filePrintln(file, "Average Error\t{:.04f}%\t\t{:.02f}ppm".format(np.mean(percent), np.mean(percent) * 10000))


def calibrateDAC(save=False, file=None):

    filePrintln(file, "\n*** DAC CALIBRATION ***\n")

    amu.query("SWEEP:TRIG:VOC?")        # make sure open circuit voltage condition is in place

    amu.send("ADC:VOLT:PGA 0")          # set voltage pga to max scale

    vmax = float(amu.query("ADC:VOLT:MAX?"))

    filePrintln(file, "Setting source meter to " + str(vmax) + " Volts\n")

    instruments.set_voltage_mode(vmax)
    instruments.set_voltage(vmax)

    time.sleep(2.0)
    # if device is AMU2.# or greater
    dac_coeff = amu.query("DAC:CAL?")                 # run calibration
    time.sleep(2)
    filePrintln(file, "DAC:CAL Response: " + dac_coeff)

    if(save==True):
        amu.send("DAC:CAL:SAVE")            # save cal value to eeprom

    time.sleep(1)
    filePrintln(file, "DAC:GAIN:CORRECTION " +  amu.query("DAC:GAIN:CORR?"))
    time.sleep(0.25)
    filePrintln(file, "DAC:OFFSET:CORRECTION " + amu.query("DAC:OFF:CORR?"))


def checkDACCal(steps=10, file=None):

    vmax = float(amu.query("ADC:VOLT:MAX?"))

    # instruments.set_current_meter(0.1, res=1e-2)
    instruments.set_voltage_mode(vmax + 1, current_limit = 0.005, res=1e-2)

    vstart = vmax * 0.95

    instruments.set_voltage(vstart)

    time.sleep(1)

    filePrintln(file, "\nSet(V)\tDAC(V)\t\tAMU(V)\t\tDelta(µV)\tDelta(%)\tDelta(ppm)")
    
    delta = []
    
    step = vstart / steps

    for i in range(steps):
        voltage_setpoint = vstart - (step * i)
        #setVoltage(voltage_setpoint)
        amu.send("DAC:VOLTAGE " + str(voltage_setpoint))
        time.sleep(1)
        instruments.measure_voltage()
        amuVoltage = float(amu.query("MEAS:ADC:VOLT?"))
        dacVoltage = float(amu.query("DAC:VOLT?"))
        delta.append(amuVoltage - dacVoltage)
        delta_uv = delta[i]*1000000
        delta_percent = abs(delta[i] / vmax * 100)
        delta_ppm = abs(delta[i] / vmax * 1000000)
        filePrintln(file, "{:<6.04f}\t{:<8.06f}\t{:<8.06f}\t{:<8.02f}\t{:<8.04}\t{:<8.02f}".format(voltage_setpoint, dacVoltage, amuVoltage, delta_uv, delta_percent, delta_ppm))
    
    mean = np.mean(delta)

    filePrintln(file, "{:.04f}V\t{:.02f}µV\t{:.04f}%\t{:.02f}ppm\n".format(vmax, mean*1000000, mean / vmax * 100, mean / vmax * 1000000))

def saveStuff():
    amu.send("ADC:SAVE:ALL:INT")
    amu.send("ADC:CH0:CAL:SAVE")
    amu.send("ADC:CH1:CAL:SAVE")

def fullCalibration(steps, propertyTag = None):


    amu.query("SWEEP:TRIG:VOC?")
    time.sleep(1)
    
    filename = amu.query("SYST:SER?") + ".cal"

    print("../cal/" + filename)

    # file = open("../cal/" + filename, 'w')
    file = None

    filePrintln(file, "AMU Serial Number:\t" + amu.query("SYST:SER?"))
    filePrintln(file, "AMU I²C Address:\t" + amu.query("SYST:TWI:ADD?"))
    filePrintln(file, "AMU Temperature:\t" + amu.query("SYST:TEMP?") + " °C")
    filePrintln(file, "Device Manufacturer:\t" + amu.query("DUT:MAN?"))
    filePrintln(file, "Device Model:\t" + amu.query("DUT:MOD?"))
    filePrintln(file, "Device Serial Number:\t" + amu.query("DUT:SER?"))
    filePrintln(file, "Device Notes:\t" + amu.query("DUT:NOTES?"))

    if propertyTag != None:
        filePrintln(file, "\nProperty Tag:\t" + str(propertyTag) + "\n")

    filePrintln(file, "\nCalibration Started on " +  datetime.now().strftime("%m/%d/%Y") + " at " + datetime.now().strftime("%H:%M:%S"))

    # ADC Calibrations for each PGA setting
    for pga in range(8):

        calibrateVoltage(pga=pga, steps=steps, save=True, file=file)

        checkVoltageCal(steps=10, file=file)

        calibrateCurrent(pga=pga, steps=steps, save=True, file=file)

        checkCurrentCal(steps=10, file=file)

    # DAC Calibration
    calibrateDAC(save=False, file=file)

    checkDACCal(file=file)

    filePrintln(file, "Calibration Ended on " +  datetime.now().strftime("%m/%d/%Y") + " at " + datetime.now().strftime("%H:%M:%S"))

    filePrintln(file, "\n*** ADS131M08 REGS ***\n")
    # printADCRegs(file)
    filePrintln(file, getIVCoefficients())

    if(file is not None):
        file.close()


def main():
    global amu  # Need global to assign to module-level variable

    # Parse command line arguments
    parser = argparse.ArgumentParser(description='AMU Noise Test Tool - Measurement and Statistical Analysis')
    parser.add_argument('--non-interactive', action='store_true', 
                       help='Run in non-interactive mode (for testing/automation)')
    parser.add_argument('--command', '-c', type=str, 
                       help='Execute a single SCPI command and exit')
    parser.add_argument('--measure', '-m', type=str, choices=['voc', 'isc', 'rtd'],
                       help='Run specific measurement type and exit')
    parser.add_argument('--points', '-p', type=int, default=200,
                       help='Number of measurement points to collect (default: 200)')
    parser.add_argument('--port', type=str, 
                       help='Specify COM port (e.g., COM4). If not specified, auto-detect AMU device.')
    args = parser.parse_args()

    # Initialize AMU connection with optional COM port specification
    print("Initializing AMU connection...")
    amu = AMU(com_port=args.port)

    if not amu.available:
        if args.port:
            print(f"AMU device not available on specified port {args.port}. Please check connection and port.")
        else:
            print("AMU device not available. Please check connection or specify COM port with --port.")
        return

    amu.send("*CLS")

    firmware = amu.query("SYST:FIRM?")
    print("AMU connected with firmware version", firmware)
    
    if ((firmware == "\"isc2.08.29.19\"") or (firmware == "\"amu.08.29.19\"")):
        print("\b\n\nTHIS FIRMWARE DOES NOT SUPPORT READING FUSES, THEREFORE FUSES HAVE NOT BEEN SET PROPERLY\n\n")

    # Handle single command execution
    if args.command:
        try:
            if '?' in args.command:
                response = amu.query(args.command)
                print(f"Response: {response}")
            else:
                amu.send(args.command)
                print("Command sent successfully")
        except Exception as e:
            print(f"Error executing command: {e}")
        finally:
            amu.disconnect()
        return

    # Handle non-interactive mode
    if args.non_interactive or not sys.stdin.isatty():
        print("Non-interactive mode detected. Use --command to execute single commands.")
        print("Available commands: info, voc, isc, coeff")
        amu.disconnect()
        return

    # Interactive mode
    user_input = 0
    print("AMU Interactive Console")
    print("Available commands: info, voc, isc, coeff, full, dac, v0-v7, i0-i7, help, exit")
    print("You can also send SCPI commands directly (use ? for queries)")

    while user_input != 'exit':
        try:
            user_input = input(">>> ").strip()
            if not user_input:  # Handle empty input
                continue
                
        except EOFError:
            print("\nEOF detected, exiting...")
            break
        except KeyboardInterrupt:
            print("\nKeyboard interrupt, exiting...")
            break
        
        if user_input == 'info':     
            info()
        elif user_input == 'full':
            fullCalibration(steps=5)
        elif user_input == 'dac':
            calibrateDAC()
        elif user_input == 'coeff':         
            print(getIVCoefficients())
        elif user_input == 'v0': calibrateVoltage(pga = 0, printResult=True)
        elif user_input == 'v1': calibrateVoltage(pga = 1, printResult=True)
        elif user_input == 'v2': calibrateVoltage(pga = 2, printResult=True)
        elif user_input == 'v3': calibrateVoltage(pga = 3, printResult=True)
        elif user_input == 'v4': calibrateVoltage(pga = 4, printResult=True)
        elif user_input == 'v5': calibrateVoltage(pga = 5, printResult=True)
        elif user_input == 'v6': calibrateVoltage(pga = 6, printResult=True)
        elif user_input == 'v7': calibrateVoltage(pga = 7, printResult=True)

        elif user_input == 'i0': calibrateCurrent(pga = 0, printResult=True)
        elif user_input == 'i1': calibrateCurrent(pga = 1, printResult=True)
        elif user_input == 'i2': calibrateCurrent(pga = 2, printResult=True)
        elif user_input == 'i3': calibrateCurrent(pga = 3, printResult=True)
        elif user_input == 'i4': calibrateCurrent(pga = 4, printResult=True)
        elif user_input == 'i5': calibrateCurrent(pga = 5, printResult=True)
        elif user_input == 'i6': calibrateCurrent(pga = 6, printResult=True)
        elif user_input == 'i7': calibrateCurrent(pga = 7, printResult=True)

        elif user_input == 'dcheck': checkDACCal()
        elif user_input == 'icheck': checkCurrentCal(steps=10)
        elif user_input == 'vcheck' : checkVoltageCal(steps=10)

        elif user_input == 'help':
            print("Available commands:")
            print("  info  - Display device information")
            print("  full  - Perform full calibration")
            print("  dac   - Calibrate DAC")
            print("  coeff - Display calibration coefficients")
            print("  v0-v7 - Calibrate voltage for PGA 0 to 7")
            print("  i0-i7 - Calibrate current for PGA 0 to 7")
            print("  help  - Show this help message")
            print("  exit  - Exit the program")
            print("  Any SCPI command (use ? for queries)")

        elif user_input != 'exit':
            if '?' in user_input:
                try:
                    response = amu.query(user_input)
                    print(response)
                except Exception as e:
                    print(f"Error executing query: {e}")
            else:
                try:
                    amu.send(user_input)
                    print("Command sent successfully")
                except Exception as e:
                    print(f"Error sending command: {e}")

    amu.disconnect()

if __name__ == "__main__":
    main()