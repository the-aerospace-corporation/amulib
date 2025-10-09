import os
import sys
import time
import winsound
from datetime import datetime
from amu import amu
import numpy as np
import teleplot as teleplot

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

    i=0
    while i < 50:

        measurement = (amu.query("SWEEP:TRIG:VOC?"))     # response should be 5.01666,28.095, need to seperate into 2 variables

        voltage = float(measurement.split(',')[0]) * 1000000.0
        temperature = float(measurement.split(',')[1])

        teleplot.sendTelemetry("voc", voltage)
        teleplot.sendTelemetry("temp", temperature)

        i += 1

def isc():


    i=0

    while i < 50:

        measurement = (amu.query("SWEEP:TRIG:ISC?"))     # response should be 5.01666,28.095, need to seperate into 2 variables    

        current = float(measurement.split(',')[0]) * 1000000.0
        temperature = float(measurement.split(',')[1])

        teleplot.sendTelemetry("isc", current)
        teleplot.sendTelemetry("temp", temperature)

        i += 1


def main():

    if amu.available:

        amu.send("*CLS")

        firmware = amu.query("SYST:FIRM?")
        print("AMU connected with firmware version", firmware)
        
        if ((firmware == "\"isc2.08.29.19\"") or (firmware == "\"amu.08.29.19\"")):
            print("\b\n\nTHIS FIRMWARE DOES NOT SUPPORT READING FUSES, THEREFORE FUSES HAVE NOT BEEN SET PROPERLY\n\n")

        user_input = 0

        while user_input != 'exit':

            user_input = input(">>> ")

            if user_input == 'info':     info()
            elif user_input == 'voc':      voc()
            elif user_input == 'isc':      isc()
            elif user_input == 'coeff':      calibration_coefficients()
            

           
            
            elif user_input != 'exit':
                if '?' in user_input:
                    print(amu.query(user_input))
                else:
                    amu.send(user_input)

        amu.disconnect()

if __name__== "__main__":
  main()