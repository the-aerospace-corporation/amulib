import os
import sys
import time
import argparse
import winsound
from datetime import datetime
from amu import amu
import numpy as np
import teleplot as teleplot
import winsound
from instruments import CalibrationInstruments
import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots


source_meter_visa = 'USB0::0x0957::0x8D18::MY51140506::INSTR'
# volt_meter_visa = 'GPIB0::1::INSTR'
# current_meter_visa = 'GPIB0::2::INSTR'

# instruments = CalibrationInstruments(source_meter_visa, volt_meter_visa, current_meter_visa)
instruments = CalibrationInstruments(source_meter_visa)


amu = amu()



def filePrintln(file, text=""):
    file.write(text + "\n")
    print(text)

def info():

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

    print("NOTES:", notes)
    print("MANUFACTURER", amu.query("DUT:MAN?"))
    print("MODEL", amu.query("DUT:MOD?"))
    print("TECH", amu.query("DUT:TECH?"))
    print("SERIAL", amu.query("DUT:SER?"))
    print("COVERGALSS", amu.query("DUT:COVER?"))
    print("JUNCTION", amu.query("DUT:JUNC?"))
    print("ENERGY", amu.query("DUT:ENERGY?"))
    print("DOSE", amu.query("DUT:DOSE?"))
    
    print("ADDRESS:", address)
    print("AMU-SERIAL:", serial)
    print("CONFIG:", config)
    
    print("DAC-CORR", amu.query("DAC:GAIN:CORR?"))


def calibration_coefficients():

    print("\tVoltage\t\t\t\tCurrent")
    print("PGA\tGain\t\tOffset\t\tGain\t\tOffset")

    for pga in range(0,8):
        
        amu.send(f"ADC:VOLT:PGA {pga}")
        amu.send(f"ADC:CURR:PGA {pga}")

        voltage_gain = int(amu.query("ADC:VOLT:GAIN?"))
        current_gain = int(amu.query("ADC:CURR:GAIN?"))

        voltage_offset = int(amu.query("ADC:VOLT:OFF?"))
        current_offset = int(amu.query("ADC:CURR:OFF?"))


        print(f"{pga}\t0x{voltage_gain:06X}\t0x{voltage_offset:06X}\t0x{current_gain:06X}\t0x{current_offset:06X}")


def voc():

    measurement = (amu.query("SWEEP:TRIG:VOC?"))     # response should be 5.01666,28.095, need to seperate into 2 variables

    voltage = float(measurement.split(',')[0])
    temperature = float(measurement.split(',')[1])

    print(f"VOC: {voltage} V, TEMP: {temperature} °C")

    teleplot.sendTelemetry("voc", voltage)
    teleplot.sendTelemetry("temp", temperature)



def isc():

    measurement = (amu.query("SWEEP:TRIG:ISC?"))     # response should be 5.01666,28.095, need to seperate into 2 variables    

    current = float(measurement.split(',')[0])
    temperature = float(measurement.split(',')[1])

    print(f"ISC: {current} A, TEMP: {temperature} °C")

    teleplot.sendTelemetry("isc", current)
    teleplot.sendTelemetry("temp", temperature)

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


def calibrateVoltage(pga, steps = 10, save=False, printResult=False):

    amu.send("ADC:VOLT:PGA " + str(pga))
    print("ADC:VOLT:PGA " + str(pga))

    amu.send("ADC:VOLT:CAL:RESET")

    amu.query("SWEEP:TRIG:VOC?")      # make sure open circuit voltage condition is in place

    vmax = float(amu.query("ADC:VOLT:MAX?").strip())

    print("DAC:STATE?", amu.query("DAC:STATE?"))
    print("DAC:VOLT:RAW?", amu.query("DAC:VOLT:RAW?"))
    print("DAC:GAIN:CORR?", amu.query("DAC:GAIN:CORR?"))
    print("DAC:OFF:CORR?", amu.query("DAC:OFF:CORR?"))
    print("ADC:VOLT:OFF?", amu.query("ADC:VOLT:OFF?"))
    print("ADC:VOLT:GAIN?", amu.query("ADC:VOLT:GAIN?"))
    instruments.setVoltageMode(vmax)

    # Set source meter to zero output voltage, adjust until meter reads <10uV
    instruments.setVoltageWithFeedback(0.000, max_error=vmax*.00001)

    print("\nVoltage Set to ZERO")
    print("DAC:STATE?", amu.query("DAC:STATE?"))
    print("DAC:VOLT:RAW?", amu.query("DAC:VOLT:RAW?"))
    print("DAC:GAIN:CORR?", amu.query("DAC:GAIN:CORR?"))
    print("DAC:OFF:CORR?", amu.query("DAC:OFF:CORR?"))
    print("ADC:VOLT:OFF?", amu.query("ADC:VOLT:OFF?"))
    print("ADC:VOLT:GAIN?", amu.query("ADC:VOLT:GAIN?"))   

    # Perform AMU offset calibration
    amu.send("ADC:VOLT:CAL:ZERO")    # maybe do twice?
    
    print("\nCAL ZERO")
    print("DAC:STATE?", amu.query("DAC:STATE?"))
    print("DAC:VOLT:RAW?", amu.query("DAC:VOLT:RAW?"))
    print("DAC:GAIN:CORR?", amu.query("DAC:GAIN:CORR?"))
    print("DAC:OFF:CORR?", amu.query("DAC:OFF:CORR?"))
    print("ADC:VOLT:OFF?", amu.query("ADC:VOLT:OFF?"))
    print("ADC:VOLT:GAIN?", amu.query("ADC:VOLT:GAIN?"))   
    # Set source meter to full scale voltage from vmax, adjust until meter reads <10uV
    instruments.setVoltageWithFeedback(vmax, max_error=vmax*.00001)
    
    print("\nVoltage Set to FULL")
    print("DAC:STATE?", amu.query("DAC:STATE?"))
    print("DAC:VOLT:RAW?", amu.query("DAC:VOLT:RAW?"))
    print("DAC:GAIN:CORR?", amu.query("DAC:GAIN:CORR?"))
    print("DAC:OFF:CORR?", amu.query("DAC:OFF:CORR?"))
    print("ADC:VOLT:OFF?", amu.query("ADC:VOLT:OFF?"))
    print("ADC:VOLT:GAIN?", amu.query("ADC:VOLT:GAIN?"))   
    
    # Perform AMU full scale calibration
    amu.send("ADC:VOLT:CAL:FULL")

    print("\nCAL FULL")
    print("DAC:STATE?", amu.query("DAC:STATE?"))
    print("DAC:VOLT:RAW?", amu.query("DAC:VOLT:RAW?"))
    print("DAC:GAIN:CORR?", amu.query("DAC:GAIN:CORR?"))
    print("DAC:OFF:CORR?", amu.query("DAC:OFF:CORR?"))
    print("ADC:VOLT:OFF?", amu.query("ADC:VOLT:OFF?"))
    print("ADC:VOLT:GAIN?", amu.query("ADC:VOLT:GAIN?"))   

    # # Check calibration:
    # if(vmax > 10):
    #     setVoltageMode(vmax, res=1e-5)
        
    # else:
    #     setVoltageMode(vmax, res=1e-6)

    instruments.setVoltageMode(vmax)

    print("Set(V)", "Meter(V)", "AMU(V)", "Delta(µV)","Delta(%)", "Delta(ppm)", sep='\t')

    delta = []
    
    step = vmax / steps

    for i in range(steps + 1):
        voltage_setpoint = vmax - (step * i)
        instruments.setVoltage(voltage_setpoint)
        time.sleep(0.1)
        meterVoltage = instruments.measureVoltage()
        amuVoltage = float(amu.query("MEAS:ADC:VOLT?"))
        delta.append(abs(meterVoltage - amuVoltage))
        print("{:.04f}\t{:.06f}\t{:.06f}\t{:.02f}\t{:.04}\t{:.02f}".format(voltage_setpoint, meterVoltage, amuVoltage, delta[i]*1000000, delta[i] / vmax * 100, delta[i] / vmax * 1000000))
    
    mean = np.mean(delta)

    cal_result = "{:.04f}V\t{:.02f}µV\t{:.04f}%\t{:.02f}ppm".format(vmax, mean*1000000, mean / vmax * 100, mean / vmax * 1000000)

    if save:
        amu.send("ADC:VOLT:CAL:SAVE")

    if printResult:
        print(cal_result)

    return cal_result

def checkVoltageCal(steps=10):

    amu.query("SWEEP:TRIG:VOC?")      # make sure open circuit voltage condition is in place

    vmax = float(amu.query("ADC:VOLT:MAX?").strip())

    if(vmax > 10):
        instruments.setVoltageMode(vmax, res=1e-4)
    else:
        instruments.setVoltageMode(vmax, res=1e-5)

    print("Set(V)", "Meter(V)", "AMU(V)", "Delta(µV)","Delta(%)", "Delta(ppm)", sep='\t')
    
    delta = []
    
    step = vmax / steps

    for i in range(steps + 1):
        voltage_setpoint = vmax - (step * i)
        instruments.setVoltage(voltage_setpoint)
        time.sleep(0.1)
        meterVoltage = instruments.measureVoltage()
        amuVoltage = float(amu.query("MEAS:ADC:VOLT?"))
        delta.append(abs(meterVoltage - amuVoltage))
        print("{:.04f}\t{:.06f}\t{:.06f}\t{:.02f}\t{:.04}\t{:.02f}".format(voltage_setpoint, meterVoltage, amuVoltage, delta[i]*1000000, delta[i] / vmax * 100, delta[i] / vmax * 1000000))
    
    mean = np.mean(delta)

    cal_result = "{:.04f}V\t{:.02f}µV\t{:.04f}%\t{:.02f}ppm".format(vmax, mean*1000000, mean / vmax * 100, mean / vmax * 1000000)
    
    print(cal_result)
    
def calibrateCurrent(pga, steps = 10, save=False, printResult=False):

    amu.send("ADC:CURR:PGA " + str(pga))
    print("ADC:CURR:PGA " + str(pga))

    amu.send("ADC:CURR:CAL:RESET")

    amu.query("SWEEP:TRIG:ISC?")      # make sure short circuit condition is in place

    imax = float(amu.query("ADC:CURR:MAX?"))

    instruments.setCurrentMode(imax)

    # Set source meter to zero output voltage, adjust until meter reads <10uV
    instruments.setCurrentWithFeedback(0.000, max_error=imax*.00001)

    # Perform AMU offset calibration
    amu.send("ADC:CURR:CAL:ZERO")    # maybe do twice?
    
    print("ADC:CURR:OFF COEFF", amu.query("ADC:CURR:OFF?"), sep='\t')

    print("DAC:STATE?", amu.query("DAC:STATE?"))
    print("DAC:VOLT:RAW?", amu.query("DAC:VOLT:RAW?"))
    print("ADC:VOLT:GAIN?", amu.query("ADC:VOLT:GAIN?"))

    # Set source meter to full scale voltage from vmax, adjust until meter reads <10uV
    instruments.setCurrentWithFeedback(imax, max_error=imax*.00001)
    
    # Perform AMU full scale calibration
    amu.send("ADC:CURR:CAL:FULL")

    print("ADC:CURR:GAIN COEFF", amu.query("ADC:VOLT:GAIN?"), sep='\t')

    # # Check calibration:
    # if(imax > 1):
    #     setCurrentMode(imax, res=1e-5)
    # else:
    #     setCurrentMode(imax, res=1e-6)

    instruments.setCurrentMode(imax)
    print("Set(A)", "Meter(A)", "AMU(A)", "Delta(µA)","Delta(%)", "Delta(ppm)", sep='\t')

    delta = []
    step = imax / steps

    for i in range(steps + 1):
        current_setpoint = imax - (step * i)
        instruments.setCurrent(current_setpoint)
        time.sleep(0.1)
        meterCurrent = instruments.measureCurrent()
        amuCurrent = float(amu.query("MEAS:ADC:CURR?"))
        delta.append(abs(meterCurrent - amuCurrent))
        print("{:.04f}\t{:.06f}\t{:.06f}\t{:.02f}\t{:.04f}\t{:.02f}".format(current_setpoint, meterCurrent, amuCurrent, delta[i]*1000000, delta[i] / imax * 100, delta[i] / imax * 1000000))

    mean = np.mean(delta)

    cal_result = "{:.04f}A\t{:.02f}µA\t{:.04f}%\t{:.02f}ppm".format(imax, mean*1000000, mean / imax * 100, mean / imax * 1000000)

    if save:
        amu.send("ADC:CURR:CAL:SAVE")
    
    if printResult:
        print(cal_result)

    return cal_result

def checkCurrentCal(steps=10):

    amu.query("SWEEP:TRIG:ISC?")      # make sure short circuit condition is in place

    imax = float(amu.query("ADC:CURR:MAX?"))

    # Check calibration:
    if(imax > 1):
        instruments.setCurrentMode(imax, res=1e-4)
    else:
        instruments.setCurrentMode(imax, res=1e-5)
    print("Set(A)", "Meter(A)", "AMU(A)", "Delta(µA)","Delta(%)", "Delta(ppm)", sep='\t')

    delta = []
    step = imax / steps

    for i in range(steps + 1):
        current_setpoint = imax - (step * i)
        instruments.setCurrent(current_setpoint)
        time.sleep(0.1)
        meterCurrent = instruments.measureCurrent()
        amuCurrent = float(amu.query("MEAS:ADC:CURR?"))
        delta.append(abs(meterCurrent - amuCurrent))
        print("{:.04f}\t{:.06f}\t{:.06f}\t{:.02f}\t{:.04f}\t{:.02f}".format(current_setpoint, meterCurrent, amuCurrent, delta[i]*1000000, delta[i] / imax * 100, delta[i] / imax * 1000000))

    mean = np.mean(delta)

    cal_result = "{:.04f}A\t{:.02f}µA\t{:.04f}%\t{:.02f}ppm".format(imax, mean*1000000, mean / imax * 100, mean / imax * 1000000)

    print(cal_result)

def calibrateDAC():

    hardwareType = int(amu.query("SYST:HARD?"))

    if hardwareType >= 32:    
        amu.send("ADC:VOLT:PGA 2")        # set PGA to 2.. most commonly used

        amu.query("SWEEP:TRIG:VOC?")      # make sure open circuit voltage condition is in place

        vmax = float(amu.query("ADC:VOLT:MAX?"))

        instruments.setVoltageMode(vmax)

        instruments.setVoltage(vmax)

        time.sleep(0.5)                    # wait for voltage to settle

    else:
        amu.send("ADC:CURR:PGA 4")        # set PGA to 4.. most commonly used

        amu.query("SWEEP:TRIG:ISC?")      # make sure short circuit current condition is in place

        imax = float(amu.query("ADC:CURR:MAX?"))

        instruments.setCurrentMode(imax)

        instruments.setCurrent(imax)

    # if device is AMU2.# or greater
    dac_cal = amu.query("DAC:CAL?")     # run calibration

    amu.send("DAC:STATE 0")             # turn off the dac so we don't start holding a load, will heat up FET

    amu.send("DAC:CAL:SAVE")            # save cal value to eeprom

    print("DAC CAL: " + dac_cal)

    return "DAC CAL: " + dac_cal

def saveStuff():

    amu.send("ADC:SAVE:ALL:INT")
    amu.send("ADC:CH0:CAL:SAVE")
    amu.send("ADC:CH1:CAL:SAVE")

def fullCalibration(steps):

    time.sleep(0.5)

    filename = amu.query("SYST:SER?") + ".cal"

    print("../cal/" + filename)

    file = open("../cal/" + filename, 'w')

    now = datetime.now()
    filePrintln(file, "Calibration Started at " +  now.strftime("%m/%d/%Y %H:%M:%S"))

    # record temperature of internal adc temperature sensor
    amu.send("ADC:CAL")         
    filePrintln(file, "\nADC:CAL: " + amu.query("ADC:CAL?"))
    
    # Internal calibrations
    amu.setCommTimeout(8)
    # amu.send("ADC:CAL:ALL:INT")
    amu.send("ADC:CH2:CAL:INT")
    amu.send("ADC:CH5:CAL:INT")
    amu.send("ADC:CH6:CAL:INT")
    amu.send("ADC:CH7:CAL:INT")
    amu.send("ADC:CH8:CAL:INT")
    amu.send("ADC:CH12:CAL:INT")
    amu.setCommTimeout(3)

    amu.send("ADC:SAVE:ALL:INTernal")   # save internal calibrations
    filePrintln(file, "\nADC:SAVE:ALL:INTernal")
      
    filePrintln(file, "\nMEAS:ADC:TSENS?: " + amu.query("MEAS:ADC:TSENS?"))

    filePrintln(file, "\n***VOLTAGE CALIBRATIONS***")
    # Voltage Calibrations
    # for i in range(0,7):
    for i in range(2,7):
        print("\n*** VOLTAGE PGA " + str(i) + " ***\n")
        filePrintln(file, calibrateVoltage(i, steps=steps, save=True))

    filePrintln(file, "\n***CURRENT CALIBRATIONS***")
    # Current Calibrations
    # for i in range(0,7):
    for i in range(4,7):
        print("\n*** CURRENT PGA " + str(i) + " ***\n")
        filePrintln(file, calibrateCurrent(i, steps=steps, save=True))

    filePrintln(file)

    # calibrate DAC
    filePrintln(file, calibrateDAC())

    filePrintln(file)

    # print coefficients
    filePrintln(file, getIVCoefficients())

    now = datetime.now()
    filePrintln(file, "Calibration Stopped at " +  now.strftime("%d/%m/%Y %H:%M:%S"))

    file.close()

    bell()

def bell():
    winsound.Beep(440, 200)
    #time.sleep(0.1)
    winsound.Beep(880, 200)
    #time.sleep(0.1)
    winsound.Beep(1760, 200)
    #time.sleep(0.1)
    winsound.Beep(880, 200)
    #time.sleep(0.1)
    winsound.Beep(440, 200)


def eyas_serial_numbers(num_devices):

    for i in range(1, num_devices+1):

        address_string = " (@{0})".format(i)

        print(amu.query("SYST:SER?" + address_string))


def eyas_read_dut(num_devices):

    column_names = ['I2C Address',
                    'Notes',
                    'Junction',
                    'Coverglass',
                    'Interconnects',
                    'Manufacturer',
                    'Model',
                    'Technology',
                    'Serial Number',
                    'Energy',
                    'Dose',
                    'Area',
                    'AM0',
                    'AMU Serial Number']

    df = pd.DataFrame(columns=range(1, len(column_names) + 1))
    df.columns = column_names

    # print("Number of columns: ", len(column_names))

    twi_address = []

    serial_number = []

    dut_junctions = []
    dut_coverglass = []
    dut_interconnect = []
    dut_manufacturer = []
    dut_model = []
    dut_technology = []
    dut_serial_number = []
    dut_energy = []
    dut_dose = []
    dut_notes = []

    sweep_config_area = []
    sweep_config_am0 = []

    for i in range(0, num_devices):

        scpi_address_string = " (@{0})".format(i+1)

        df.at[i, 'I2C Address'] = amu.query("SYST:TWI:ADD?" + scpi_address_string)

        df.at[i, 'Notes'] = amu.query("DUT:NOTES?" + scpi_address_string)
        df.at[i, 'Junction'] = amu.query("DUT:JUNC?" + scpi_address_string)
        df.at[i, 'Coverglass'] = amu.query("DUT:COVER?" + scpi_address_string)
        df.at[i, 'Interconnects'] = amu.query("DUT:INTER?" + scpi_address_string)
        df.at[i, 'Manufacturer'] = amu.query("DUT:MAN?" + scpi_address_string)
        df.at[i, 'Model'] = amu.query("DUT:MOD?" + scpi_address_string)
        df.at[i, 'Technology'] = amu.query("DUT:TECH?" + scpi_address_string)
        df.at[i, 'Serial Number'] = amu.query("DUT:SER?" + scpi_address_string)
        df.at[i, 'Energy'] = amu.query("DUT:ENERGY?" + scpi_address_string)
        df.at[i, 'Dose'] = amu.query("DUT:DOSE?" + scpi_address_string)

        df.at[i, 'Area'] = amu.query("SWEEP:CONFIG:AREA?" + scpi_address_string)
        df.at[i, 'AM0'] = amu.query("SWEEP:CONFIG:AM0?" + scpi_address_string)

        df.at[i, 'AMU Serial Number'] = amu.query("SYST:SER?" + scpi_address_string)


    print(df)

def eyas_plot_sweeps(num_devices):

    fig = go.Figure()


    for i in range(0, num_devices):

        scpi_address_string = " (@{0})".format(i+1)

        voltage = amu.query("READ:SWEEP:VOLT?" + scpi_address_string).split(",")
        current = amu.query("READ:SWEEP:CURR?" + scpi_address_string).split(",")


        # print(i)
        # print(voltage)
        # print(current)
        # print(" ")

        fig.add_trace(go.Scatter(x=voltage,
                            y=current,
                            mode='markers+lines',
                            name=scpi_address_string
                            ))

    fig.update_layout(title="LIV For {0} AMUs on Eyas Payload".format(num_devices),
                    xaxis_title='Voltage (V)',
                    yaxis_title="Current (A)",
                    template="plotly_dark",
                    width=1200,
                    height=800,
                    showlegend=True)

    fig.show()


def main():
    # Parse command line arguments
    parser = argparse.ArgumentParser(description='AMU Tool - Interactive SCPI interface for AMU devices')
    parser.add_argument('--non-interactive', action='store_true', 
                       help='Run in non-interactive mode (for testing/automation)')
    parser.add_argument('--command', '-c', type=str, 
                       help='Execute a single SCPI command and exit')
    args = parser.parse_args()

    if not amu.available:
        print("AMU device not available. Please check connection.")
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
    print("Available commands: info, voc, isc, coeff, help, exit")
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
        elif user_input == 'voc':      
            voc()
        elif user_input == 'isc':      
            isc()
        elif user_input == 'coeff':      
            calibration_coefficients()

        ### Flight Computer Functions
        elif user_input == 'eyas_serial':
            eyas_serial_numbers(12)
        elif user_input == 'eyas_read_dut':
            eyas_read_dut(12)
        elif user_input == 'eyas_plot_sweeps':
            eyas_plot_sweeps(12)
            
        elif user_input == 'help':
            print("Available commands:")
            print("  info  - Display device information")
            print("  voc   - Measure open circuit voltage")
            print("  isc   - Measure short circuit current")
            print("  coeff - Display calibration coefficients")
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