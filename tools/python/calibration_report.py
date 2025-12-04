import sys
import time
import argparse
import os
from datetime import datetime
from amu import amu as AMU
import numpy as np
import pandas as pd
import plotly.graph_objects as go
import plotly.subplots as sp
from plotly.subplots import make_subplots
import plotly.express as px
import webbrowser
import tempfile
from instruments import CalibrationInstruments

    
# Try weasyprint (but don't fail if it's not working)
try:
    from weasyprint import HTML, CSS
    WEASYPRINT_AVAILABLE = True
except (ImportError, OSError) as e:
    WEASYPRINT_AVAILABLE = False
    if "pango" in str(e).lower():
        print("Note: WeasyPrint available but Pango library missing. Using alternative PDF generation.")


# Configuration
source_meter_visa = 'USB0::0x0957::0x8D18::MY51140506::0::INSTR'
# volt_meter_visa = 'GPIB0::1::INSTR'  
# current_meter_visa = 'GPIB0::2::INSTR'

instruments = CalibrationInstruments(source_meter_visa)
amu = None

class CalibrationReport:
    """Class to handle calibration data collection and report generation"""
    
    @staticmethod
    def format_register_value(value, digits=6):
        """Format register value as 'Hex (Decimal)' """
        try:
            # Convert to integer (handles string input from AMU queries)
            int_val = int(value)

            n24 = int_val & 0xFFFFFF
            signed_24bit = n24 if n24 < 0x800000 else n24 - 0x1000000
            return f"{signed_24bit} [0x{n24:0{digits}X}]"

        except (ValueError, TypeError):
            # If conversion fails, return original value
            return str(value)
    
    @staticmethod
    def get_sweep_type_name(type_value):
        """Convert sweep type number to descriptive name"""
        sweep_types = {
            0: "Linear (Voc to Isc)",
            1: "Logarithmic-Linear (Voc to Isc)",
            2: "User-Defined Algorithm 0",
            3: "User-Defined Algorithm 1", 
            4: "User-Defined Algorithm 2",
            5: "User-Defined Algorithm 3",
            6: "User-Defined Algorithm 4",
            7: "User-Defined Algorithm 5",
            8: "Sun Sensor Only",
            9: "Sun Sensor + Temperature",
            10: "Noise IV Test",
            11: "Noise Voltage Test",
            12: "Noise Current Test",
            13: "EEPROM Simulation"
        }
        
        try:
            type_num = int(type_value)
            return sweep_types.get(type_num, f"Unknown Type ({type_num})")
        except (ValueError, TypeError):
            return f"Invalid Type ({type_value})"
    
    def __init__(self, property_tag=None):
        self.device_info = {}
        self.voltage_coefficients = {}
        self.current_coefficients = {}
        self.voltage_accuracy_data = {}
        self.current_accuracy_data = {}
        self.dac_coefficients = {}
        self.dac_accuracy_data = {}
        self.timestamp = datetime.now()
        self.property_tag = property_tag
        
    def collect_device_info(self):
        """Collect all device information"""
        print("Collecting device information...")
        
        try:
            hardware_version = int(amu.query("SYST:HARD?"))
            hardware_names = {
                0x01: "isc² - 0.1",
                0x10: "AMU-128X - 1.0", 
                0x11: "AMU-128X - 1.1",
                0x20: "AMU-128X - 2.0",
                0x21: "AMU-128X - 2.1", 
                0x30: "AMU-128X - 3.0",
                0x31: "AMU-ESP32 - 3.1",
                0x32: "AMU-ESP32 - 3.2",
                0x33: "AMU-ESP32 - 3.3",
                0x82: "AMU-ESP32 - Dev"
            }
            
            self.device_info = {
                'hardware_version': hardware_version,
                'hardware_name': hardware_names.get(hardware_version, f"Unknown (0x{hardware_version:02X})"),
                'firmware': amu.query("SYST:FIRM?").strip('"'),
                'amu_serial': amu.query("SYST:SER?"),
                'i2c_address': amu.query("SYST:TWI:ADD?"),
                'syst_temp': amu.query("SYST:TEMP?"),
                'rtd_temp': f"{float(amu.query('MEAS:ADC:TSENS?')):.2f}",
                'sweep_config': amu.query("SWEEP:CONFIG?"),
                'voltage_pga': amu.query("ADC:VOLT:PGA?"),
                'voltage_max': amu.query("ADC:VOLT:MAX?"),
                'current_pga': amu.query("ADC:CURR:PGA?"),
                'current_max': amu.query("ADC:CURR:MAX?"),
                'dut_notes': amu.query("DUT:NOTES?"),
                'dut_manufacturer': amu.query("DUT:MAN?"),
                'dut_model': amu.query("DUT:MOD?"),
                'dut_technology': amu.query("DUT:TECH?"),
                'dut_serial': amu.query("DUT:SER?"),
                'dut_coverglass': amu.query("DUT:COVER?"),
                'dut_junction': amu.query("DUT:JUNC?"),
                'dut_energy': amu.query("DUT:ENERGY?"),
                'dut_dose': amu.query("DUT:DOSE?")
            }

            # print device_info
            for key, value in self.device_info.items():
                print(f"  {key}: {value}")
            
            # Collect source meter calibration reference information
            try:
                # Get source meter identification
                source_meter_idn = instruments.source_meter.query("*IDN?").strip()
                # Parse IDN response: Manufacturer,Model,SerialNumber,FirmwareVersion
                idn_parts = source_meter_idn.split(',')
                if len(idn_parts) >= 3:
                    source_meter_model = idn_parts[1].strip()
                    source_meter_serial = idn_parts[2].strip()
                else:
                    source_meter_model = "Unknown"
                    source_meter_serial = "Unknown"
                
                # Build calibration reference string
                calibration_ref = f"{source_meter_model} S/N:{source_meter_serial}"
                if self.property_tag:
                    calibration_ref += f" (Property Tag: {self.property_tag})"
                
                self.device_info['calibration_reference'] = calibration_ref
                self.device_info['source_meter_model'] = source_meter_model
                self.device_info['source_meter_serial'] = source_meter_serial
                
            except Exception as source_meter_error:
                print(f"Error collecting source meter info: {source_meter_error}")
                calibration_ref = "Source meter information unavailable"
                if self.property_tag:
                    calibration_ref += f" (Property Tag: {self.property_tag})"
                self.device_info['calibration_reference'] = calibration_ref
                
        except Exception as e:
            print(f"Error collecting device info: {e}")

    def calibrate_and_collect_voltage_data(self, pga, steps=10, save_data=True, calibrate=True):
        """Calibrate voltage for specific PGA and collect accuracy data"""
        print(f"Calibrating voltage PGA {pga}...")
        
        try:
            # Set up for voltage calibration
            amu.query("SWEEP:TRIG:VOC?")
            time.sleep(0.5)
            amu.send("ADC:VOLT:PGA " + str(pga))
            vmax = float(amu.query("ADC:VOLT:MAX?").strip())
            
            if calibrate:
                # Store initial coefficients
                initial_offset = amu.query("ADC:VOLT:OFF?")
                initial_gain = amu.query("ADC:VOLT:GAIN?")
                
                # Set up instruments
                instruments.set_voltage_mode(vmax, res=0.0001)
                
                # Zero calibration
                instruments.set_voltage_with_feedback(0.000, max_error=vmax*0.0001)
                time.sleep(0.5)
                amu.send("ADC:VOLT:CAL:ZERO", error_check=False)
                time.sleep(2.5)
                
                zero_offset = amu.query("ADC:VOLT:OFF?")
                zero_gain = amu.query("ADC:VOLT:GAIN?")
                
                # Full scale calibration  
                instruments.set_voltage_with_feedback(vmax * 0.98, max_error=vmax*0.000001)
                time.sleep(0.5)
                amu.send("ADC:VOLT:CAL:FULL", error_check=False)
                time.sleep(2.5)
            
                final_offset = amu.query("ADC:VOLT:OFF?")
                final_gain = amu.query("ADC:VOLT:GAIN?")
                
                # Store coefficients
                self.voltage_coefficients[pga] = {
                    'pga': pga,
                    'vmax': vmax,
                    'initial_offset': initial_offset,
                    'initial_gain': initial_gain,
                    'zero_offset': zero_offset,
                    'zero_gain': zero_gain,
                    'final_offset': final_offset,
                    'final_gain': final_gain
                }
            else:

                final_offset = amu.query("ADC:VOLT:OFF?")
                final_gain = amu.query("ADC:VOLT:GAIN?")
                
                # Store coefficients
                self.voltage_coefficients[pga] = {
                    'pga': pga,
                    'vmax': vmax,
                    'initial_offset': final_offset,
                    'initial_gain': final_gain,
                    'zero_offset': final_offset,
                    'zero_gain': final_gain,
                    'final_offset': final_offset,
                    'final_gain': final_gain
                }
            
            # Save calibration
            if save_data:
                amu.send("ADC:VOLT:CAL:SAVE")
            
            # Collect accuracy data
            self.collect_voltage_accuracy_data(pga, steps)
            
        except Exception as e:
            print(f"Error in voltage calibration PGA {pga}: {e}")

    def collect_voltage_accuracy_data(self, pga, steps=10):
        """Collect voltage accuracy verification data"""
        print(f"Collecting voltage accuracy data for PGA {pga}...")
        
        try:
            amu.query("SWEEP:TRIG:VOC?")
            time.sleep(0.5)
            amu.send("ADC:VOLT:PGA " + str(pga))
            vmax = float(amu.query("ADC:VOLT:MAX?").strip())
            
            instruments.set_voltage_mode(vmax, res='MIN', current_limit=0.005)
            
            accuracy_data = []
            vstart = vmax * 0.98
            step_size = vstart / steps
            
            print("Set(V)\tMeter(V)\tAMU(V)\t\tDelta(µV)\tDelta(%)\tDelta(ppm)")
    

            for i in range(steps + 1):
                voltage_setpoint = vstart - (step_size * i)
                instruments.set_voltage(voltage_setpoint)
                time.sleep(0.25)
                
                meter_voltage = instruments.measure_voltage()
                amu_voltage = float(amu.query("MEAS:ADC:VOLT?"))
                
                delta = meter_voltage - amu_voltage  # Keep the sign! Positive = AMU reads low, Negative = AMU reads high
                delta_uV = delta * 1000000
                delta_percent = delta / vmax * 100 if vmax != 0 else 0
                delta_ppm = delta_percent * 10000
                
                accuracy_data.append({
                    'setpoint': voltage_setpoint,
                    'meter_reading': meter_voltage,
                    'amu_reading': amu_voltage,
                    'delta': delta,
                    'delta_uV': delta_uV,
                    'delta_percent': delta_percent,
                    'delta_ppm': delta_ppm
                })

                print("{:<6.04f}\t{:<8.06f}\t{:<8.06f}\t{:<8.02f}\t{:<8.04}\t{:<8.02f}".format(voltage_setpoint, meter_voltage, amu_voltage, delta_uV, delta_percent, delta_percent * 10000))

            avg_delta = np.mean([abs(d['delta_uV']) for d in accuracy_data])  # Use abs for average magnitude
            avg_percent = np.mean([abs(d['delta_percent']) for d in accuracy_data])
            avg_error_ppm = np.mean([abs(d['delta_ppm']) for d in accuracy_data])

            self.voltage_accuracy_data[pga] = {
                'pga': pga,
                'vmax': vmax,
                'data': accuracy_data,
                'avg_delta_uV': avg_delta,
                'avg_error_percent': avg_percent,
                'avg_error_ppm': avg_error_ppm
            }

            print("\nVmax\t{:.04f}".format(vmax))
            print("Average Delta (µV)\t{:.04f}".format(avg_delta))
            print("Average Error\t{:.04f}%\t{:.02f}ppm\n".format(avg_percent, avg_error_ppm))
            
        except Exception as e:
            print(f"Error collecting voltage accuracy data PGA {pga}: {e}")
    
    def calibrate_and_collect_current_data(self, pga, steps=10, save_data=True, calibrate=True):
        """Calibrate current for specific PGA and collect accuracy data"""
        print(f"Calibrating current PGA {pga}...")
        
        try:
            # Set up for current calibration
            amu.query("SWEEP:TRIG:ISC?")
            time.sleep(0.5)
            amu.send("ADC:CURR:PGA " + str(pga))
            time.sleep(0.25)
            imax = float(amu.query("ADC:CURR:MAX?"))
            
            if calibrate:
                # Store initial coefficients
                initial_offset = amu.query("ADC:CURR:OFF?")
                initial_gain = amu.query("ADC:CURR:GAIN?")
                
                # Set up instruments
                instruments.set_current_mode(imax, res='MIN')
                
                # Zero calibration
                instruments.set_current_with_feedback(0.000, max_error=imax*0.00001)
                time.sleep(0.5)
                amu.send("ADC:CURR:CAL:ZERO", error_check=False)
                time.sleep(2.5)
                
                zero_offset = amu.query("ADC:CURR:OFF?")
                zero_gain = amu.query("ADC:CURR:GAIN?")
                
                # Full scale calibration
                instruments.set_current_with_feedback(imax * 0.98, max_error=imax*0.00001)
                time.sleep(0.5)
                amu.send("ADC:CURR:CAL:FULL", error_check=False)
                time.sleep(2.5)
            
                final_offset = amu.query("ADC:CURR:OFF?")
                final_gain = amu.query("ADC:CURR:GAIN?")
                
                # Store coefficients
                self.current_coefficients[pga] = {
                    'pga': pga,
                    'imax': imax,
                    'initial_offset': initial_offset,
                    'initial_gain': initial_gain,
                    'zero_offset': zero_offset,
                    'zero_gain': zero_gain,
                    'final_offset': final_offset,
                    'final_gain': final_gain
                }
            
            else:   

                final_offset = amu.query("ADC:CURR:OFF?")
                final_gain = amu.query("ADC:CURR:GAIN?")
                
                # Store coefficients
                self.current_coefficients[pga] = {
                    'pga': pga,
                    'imax': imax,
                    'initial_offset': final_offset,
                    'initial_gain': final_gain,
                    'zero_offset': final_offset,
                    'zero_gain': final_gain,
                    'final_offset': final_offset,
                    'final_gain': final_gain
                }


            # Save calibration
            if save_data:
                amu.send("ADC:CURR:CAL:SAVE")
            
            # Collect accuracy data
            self.collect_current_accuracy_data(pga, steps)
            
        except Exception as e:
            print(f"Error in current calibration PGA {pga}: {e}")
    
    def collect_current_accuracy_data(self, pga, steps=10):
        """Collect current accuracy verification data"""
        print(f"Collecting current accuracy data for PGA {pga}...")
        
        try:
            amu.query("SWEEP:TRIG:ISC?")
            time.sleep(0.5)
            amu.send("ADC:CURR:PGA " + str(pga))
            imax = float(amu.query("ADC:CURR:MAX?").strip())
            
            instruments.set_current_mode(imax, res='MIN', voltage_limit=5.00)
            
            accuracy_data = []
            istart = imax * 0.98
            step_size = istart / steps

            print("Set(A)\tMeter(A)\tAMU(A)\t\tDelta(µA)\tDelta(%)\tDelta(ppm)")
            
            for i in range(steps + 1):
                current_setpoint = istart - (step_size * i)
                instruments.set_current(current_setpoint)
                time.sleep(0.25)
                
                meter_current = instruments.measure_current()
                amu_current = float(amu.query("MEAS:ADC:CURR?"))
                
                delta = meter_current - amu_current  # Keep the sign! Positive = AMU reads low, Negative = AMU reads high
                delta_uA = delta * 1000000
                delta_percent = (delta / imax * 100) if imax != 0 else 0
                delta_ppm = delta_percent * 10000
                
                accuracy_data.append({
                    'setpoint': current_setpoint,
                    'meter_reading': meter_current,
                    'amu_reading': amu_current,
                    'delta': delta,
                    'delta_uA': delta_uA,
                    'delta_percent': delta_percent,
                    'delta_ppm': delta_ppm
                })

                print("{:<6.04f}\t{:<8.06f}\t{:<8.06f}\t{:<8.02f}\t{:<8.04}\t{:<8.02f}".format(current_setpoint, meter_current, amu_current, delta_uA, delta_percent, delta_percent * 10000))

            avg_delta = np.mean([abs(d['delta_uA']) for d in accuracy_data])  # Use abs for average magnitude
            avg_percent = np.mean([abs(d['delta_percent']) for d in accuracy_data])
            avg_error_ppm = np.mean([abs(d['delta_ppm']) for d in accuracy_data])
            
            self.current_accuracy_data[pga] = {
                'pga': pga,
                'imax': imax,
                'data': accuracy_data,
                'avg_delta_uA': avg_delta,
                'avg_error_percent': avg_percent,
                'avg_error_ppm': avg_error_ppm
            }

            print("\nImax\t{:.06f}".format(imax))
            print("Average Delta (µA)\t{:.04f}".format(avg_delta))
            print("Average Error\t{:.04f}%\t{:.02f}ppm\n".format(avg_percent, avg_error_ppm))
            
        except Exception as e:
            print(f"Error collecting current accuracy data PGA {pga}: {e}")

    def calibrate_and_collect_dac_data(self, steps=10, save_data=True, calibrate=True):
        """Calibrate DAC and collect accuracy data"""
        print("Calibrating DAC...")
        
        try:
            # Set up for DAC calibration

            amu.send("ADC:VOLT:PGA 0")
            time.sleep(0.25)
            vmax = float(amu.query("ADC:VOLT:MAX?"))
            time.sleep(0.10)

            amu.query("SWEEP:TRIG:VOC?")
            time.sleep(0.50)
            

            if calibrate:

                # Store initial coefficients
                initial_gain_corr = amu.query("DAC:GAIN:CORR?")
                initial_offset_corr = amu.query("DAC:OFF:CORR?")

                # Set up instruments and calibrate
                instruments.set_voltage_mode(vmax)
                instruments.set_voltage(vmax)
                time.sleep(2.0)
                
                dac_response = amu.query("DAC:CAL?")
                time.sleep(2)
            
                final_gain_corr = amu.query("DAC:GAIN:CORR?")
                final_offset_corr = amu.query("DAC:OFF:CORR?")
                
                # Store coefficients
                self.dac_coefficients = {
                    'vmax': vmax,
                    'initial_gain_correction': initial_gain_corr,
                    'initial_offset_correction': initial_offset_corr,
                    'calibration_response': dac_response,
                    'final_gain_correction': final_gain_corr,
                    'final_offset_correction': final_offset_corr
                }
            
            else:

                final_gain_corr = amu.query("DAC:GAIN:CORR?")
                final_offset_corr = amu.query("DAC:OFF:CORR?")
                
                # Store coefficients
                self.dac_coefficients = {
                    'vmax': vmax,
                    'initial_gain_correction': final_gain_corr,
                    'initial_offset_correction': final_offset_corr,
                    'calibration_response': 'N/A',
                    'final_gain_correction': final_gain_corr,
                    'final_offset_correction': final_offset_corr
                }

            if save_data:
                amu.send("DAC:CAL:SAVE") # save cal value to eeprom

            # Collect accuracy data
            self.collect_dac_accuracy_data(steps)
            
        except Exception as e:
            print(f"Error in DAC calibration: {e}")
    
    def collect_dac_accuracy_data(self, steps=10):
        """Collect DAC accuracy verification data"""
        print("Collecting DAC accuracy data...")
        
        try:
            vmax = float(amu.query("ADC:VOLT:MAX?"))
            instruments.set_voltage_mode(vmax + 1, current_limit=0.005, res=1e-2)
            
            accuracy_data = []
            vstart = vmax * 0.95
            step_size = vstart / steps

            instruments.set_voltage(vstart)

            time.sleep(1)

            print("\nSet(V)\tDAC(V)\t\tAMU(V)\t\tDelta(µV)\tDelta(%)\tDelta(ppm)")
            
            for i in range(steps):
                voltage_setpoint = vstart - (step_size * i)
                amu.send("DAC:VOLTAGE " + str(voltage_setpoint))
                time.sleep(1)
                
                instruments.measure_voltage()
                dac_voltage = float(amu.query("DAC:VOLT?"))
                time.sleep(0.10)
                amu_voltage = float(amu.query("MEAS:ADC:VOLT?"))
                
                
                delta = amu_voltage - dac_voltage
                delta_uV = delta * 1000000
                delta_percent = abs(delta / vmax * 100) if vmax != 0 else 0
                delta_ppm = (delta / vmax * 1000000) if vmax != 0 else 0  # Keep sign, show ppm relative to full scale
                
                accuracy_data.append({
                    'setpoint': voltage_setpoint,
                    'dac_reading': dac_voltage,
                    'amu_reading': amu_voltage,
                    'delta': delta,
                    'delta_uV': delta_uV,
                    'delta_percent': delta_percent,
                    'delta_ppm': delta_ppm
                })

                print("{:<6.04f}\t{:<8.06f}\t{:<8.06f}\t{:<8.02f}\t{:<8.04}\t{:<8.02f}".format(voltage_setpoint, dac_voltage, amu_voltage, delta_uV, delta_percent, delta_ppm))

            
            avg_delta = np.mean([abs(d['delta']) for d in accuracy_data])
            avg_delta_uV = avg_delta * 1000000
            avg_error_percent = avg_delta / vmax * 100 if vmax != 0 else 0
            avg_error_ppM = avg_delta / vmax * 1000000 if vmax != 0 else 0
            
            self.dac_accuracy_data = {
                'vmax': vmax,
                'data': accuracy_data,
                'avg_delta': avg_delta,
                'avg_delta_uV': avg_delta_uV,
                'avg_error_percent': avg_error_percent,
                'avg_error_ppm': avg_error_ppM
            }

            print("{:.04f}V\t{:.02f}µV\t{:.04f}%\t{:.02f}ppm\n".format(vmax, avg_delta_uV, avg_error_percent, avg_error_ppM))
            
        except Exception as e:
            print(f"Error collecting DAC accuracy data: {e}")
    
    def generate_plots(self):
        """Generate all calibration plots"""
        plots = {}
        
        # ADC Voltage Accuracy Plot - Single plot with all 8 PGA datasets
        voltage_fig = go.Figure()
        
        for pga in range(8):
            if pga in self.voltage_accuracy_data:
                data = self.voltage_accuracy_data[pga]['data']
                vmax = self.voltage_accuracy_data[pga]['vmax']
                
                # Normalize to 0-1 scale (percentage of full scale)
                normalized_setpoints = [d['setpoint'] / vmax for d in data]
                # errors = [d['delta'] for d in data]  # Use absolute error in volts
                errors_ppm = [d['delta_ppm'] for d in data]  # Use ppm error like in original script
                
                voltage_fig.add_trace(go.Scatter(
                    x=normalized_setpoints,
                    y=errors_ppm,
                    mode='lines+markers',
                    name=f'PGA {pga} (Vmax={vmax:.3f}V)',
                    line=dict(width=2),
                    marker=dict(size=6)
                ))
        
        voltage_fig.update_layout(
            title="ADC Voltage Calibration Accuracy - All PGA Settings",
            xaxis_title="Normalized Full Scale (0-1)",
            yaxis_title="Measurement Error (ppm)",
            height=500,
            showlegend=True,
            legend=dict(x=1.02, y=1),
            margin=dict(r=150)
        )
        plots['voltage_accuracy'] = voltage_fig
        
        # ADC Current Accuracy Plot - Single plot with all 8 PGA datasets
        current_fig = go.Figure()
        
        for pga in range(8):
            if pga in self.current_accuracy_data:
                data = self.current_accuracy_data[pga]['data']
                imax = self.current_accuracy_data[pga]['imax']
                
                # Normalize to 0-1 scale (percentage of full scale)
                normalized_setpoints = [d['setpoint'] / imax for d in data]
                # errors = [d['delta'] for d in data]  # Use absolute error in amps
                errors_ppm = [d['delta_ppm'] for d in data]  # Use ppm error like in original script
                
                current_fig.add_trace(go.Scatter(
                    x=normalized_setpoints,
                    y=errors_ppm,
                    mode='lines+markers',
                    name=f'PGA {pga} (Imax={imax:.6f}A)',
                    line=dict(width=2),
                    marker=dict(size=6)
                ))
        
        current_fig.update_layout(
            title="ADC Current Calibration Accuracy - All PGA Settings",
            xaxis_title="Normalized Full Scale (0-1)",
            yaxis_title="Measurement Error (ppm)",
            height=500,
            showlegend=True,
            legend=dict(x=1.02, y=1),
            margin=dict(r=150)
        )
        plots['current_accuracy'] = current_fig
        
        # DAC Accuracy Plot - Voltage setpoint vs error in ppm
        if self.dac_accuracy_data:
            data = self.dac_accuracy_data['data']
            setpoints = [d['setpoint'] for d in data]
            errors_ppm = [d['delta_ppm'] for d in data]  # Use ppm error like in original script
            
            dac_fig = go.Figure()
            dac_fig.add_trace(go.Scatter(
                x=setpoints, 
                y=errors_ppm, 
                mode='lines+markers',
                name='DAC Accuracy',
                line=dict(width=2, color='red'),
                marker=dict(size=6, color='red')
            ))
            
            dac_fig.update_layout(
                title="DAC Calibration Accuracy",
                xaxis_title="Voltage Setpoint (V)",
                yaxis_title="Measurement Error (ppm)",
                height=400,
                showlegend=False
            )
            plots['dac_accuracy'] = dac_fig
        
        return plots
    
    def generate_html_report(self, filename=None):
        """Generate comprehensive HTML calibration report"""
        if filename is None:
            filename = f"calibration_report_{self.device_info.get('amu_serial', 'unknown')}_{self.timestamp.strftime('%Y%m%d_%H%M%S')}.html"
        
        plots = self.generate_plots()
        
        # Create HTML content
        html_content = f"""
        <!DOCTYPE html>
        <html>
        <head>
            <title>AMU Calibration Report</title>
            <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
            <style>
                body {{ font-family: Arial, sans-serif; margin: 40px; }}
                .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
                .section {{ margin: 30px 0; }}
                .table {{ width: 100%; border-collapse: collapse; }}
                .table th, .table td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
                .table th {{ background-color: #f2f2f2; }}
                .plot-container {{ margin: 20px 0; }}
            </style>
        </head>
        <body>
        
        <div class="header">
            <h1>AMU Calibration Report</h1>
            <p><strong>Generated:</strong> {self.timestamp.strftime('%Y-%m-%d %H:%M:%S')}</p>
            <p><strong>Device:</strong> {self.device_info.get('hardware_name', 'Unknown')}</p>
            <p><strong>Serial Number:</strong> {self.device_info.get('amu_serial', 'Unknown')}</p>
            <p><strong>Calibration Reference:</strong> {self.device_info.get('calibration_reference', 'Unknown')}</p>
        </div>
        
        <div class="section">
            <h2>Device Information</h2>
            <table class="table">
                <tr><td><strong>Hardware Version</strong></td><td>{self.device_info.get('hardware_name', 'Unknown')}</td></tr>
                <tr><td><strong>Firmware</strong></td><td>{self.device_info.get('firmware', 'Unknown')}</td></tr>
                <tr><td><strong>AMU Serial</strong></td><td>{self.device_info.get('amu_serial', 'Unknown')}</td></tr>
                <tr><td><strong>I2C Address</strong></td><td>{self.format_register_value(str(self.device_info.get('i2c_address', 'Unknown')).strip(), digits=2)}</td></tr>
                <tr><td><strong>Temperature</strong></td><td>{self.device_info.get('rtd_temp', 'Unknown')} &deg;C</td></tr>
                <tr><td><strong>System Temperature</strong></td><td>{self.device_info.get('syst_temp', 'Unknown')} &deg;C</td></tr>
                <tr><td><strong>Notes</strong></td><td>{self.device_info.get('dut_notes', 'Unknown')}</td></tr>
            </table>
        </div>
        
        <div class="section">
            <h2>AMU Configuration</h2>
            <table class="table">
        """
        
        # Parse SWEEP:CONFIG? response and add to table
        try:
            sweep_config_raw = self.device_info.get('sweep_config', '')
            if sweep_config_raw:
                # Parse the comma-separated values from SWEEP:CONFIG?
                # Expected format: type,numPoints,delay,ratio,power,dac_gain,sweep_averages,adc_averages,am0,area
                config_values = sweep_config_raw.strip().split(',')
                
                if len(config_values) >= 10:
                    html_content += f"""
                <tr><td><strong>Sweep Type</strong></td><td>{self.get_sweep_type_name(config_values[0])}</td></tr>
                <tr><td><strong>Number of Points</strong></td><td>{config_values[1]}</td></tr>
                <tr><td><strong>Delay (ms)</strong></td><td>{config_values[2]}</td></tr>
                <tr><td><strong>Ratio</strong></td><td>{config_values[3]}</td></tr>
                <tr><td><strong>Power Setting</strong></td><td>{config_values[4]}</td></tr>
                <tr><td><strong>DAC Gain</strong></td><td>{config_values[5]}</td></tr>
                <tr><td><strong>Sweep Averages</strong></td><td>{config_values[6]}</td></tr>
                <tr><td><strong>ADC Averages</strong></td><td>{config_values[7]}</td></tr>
                <tr><td><strong>AM0 (A/m&sup2;)</strong></td><td>{float(config_values[8]):.6f}</td></tr>
                <tr><td><strong>Area (cm&sup2;)</strong></td><td>{float(config_values[9]):.4f}</td></tr>
                    """
                else:
                    html_content += f"""
                <tr><td><strong>Raw Configuration</strong></td><td>{sweep_config_raw}</td></tr>
                    """
            else:
                html_content += f"""
                <tr><td><strong>Configuration</strong></td><td>Not available</td></tr>
                """
        except Exception as e:
            html_content += f"""
                <tr><td><strong>Configuration Error</strong></td><td>Could not parse: {str(e)}</td></tr>
                """
        
        # Add PGA settings
        try:
            voltage_pga = self.device_info.get('voltage_pga', 'Unknown')
            voltage_max = self.device_info.get('voltage_max', 'Unknown')
            current_pga = self.device_info.get('current_pga', 'Unknown')
            current_max = self.device_info.get('current_max', 'Unknown')
            
            html_content += f"""
                <tr><td><strong>Voltage PGA Setting</strong></td><td>PGA {voltage_pga} (Max: {float(voltage_max):.4f} V)</td></tr>
                <tr><td><strong>Current PGA Setting</strong></td><td>PGA {current_pga} (Max: {float(current_max):.6f} A)</td></tr>
                """
        except Exception as e:
            html_content += f"""
                <tr><td><strong>PGA Settings Error</strong></td><td>Could not parse: {str(e)}</td></tr>
                """
        
        html_content += """
            </table>
        </div>
        """
        
        # Add plots
        for plot_name, fig in plots.items():
            plot_html = fig.to_html(include_plotlyjs=False, div_id=f"plot_{plot_name}")
            html_content += f"""
            <div class="section">
                <div class="plot-container">
                    {plot_html}
                </div>
            </div>
            """
        
        # Add coefficient tables
        if self.voltage_coefficients:
            html_content += """
            <div class="section">
                <h2>Voltage Calibration Coefficients</h2>
                <table class="table">
                    <tr><th>PGA</th><th>Vmax</th><th>Offset Register</th><th>Gain Register</th><th>Avg Error (&micro;V)</th></tr>
            """
            # Get current active voltage PGA setting
            active_voltage_pga = self.device_info.get('voltage_pga', None)
            try:
                active_voltage_pga = int(active_voltage_pga) if active_voltage_pga else None
            except (ValueError, TypeError):
                active_voltage_pga = None
            
            for pga in sorted(self.voltage_coefficients.keys()):
                coeff = self.voltage_coefficients[pga]
                accuracy = self.voltage_accuracy_data.get(pga, {})
                # avg_ppm = accuracy.get('avg_error_ppm', 0)
                avg_delta_uV = accuracy.get('avg_delta_uV', 0)
                
                # Make row bold if this is the active PGA setting
                row_style = 'style="font-weight: bold;"' if pga == active_voltage_pga else ''
                
                html_content += f"""
                    <tr {row_style}>
                        <td>{pga} - {2**pga}X</td>
                        <td>{coeff['vmax']:.4f}</td>
                        <td>{self.format_register_value(coeff['final_offset'])}</td>
                        <td>{self.format_register_value(coeff['final_gain'])}</td>
                        <td>{avg_delta_uV:.2f}</td>
                    </tr>
                """
            html_content += "</table></div>"
        
        if self.current_coefficients:
            html_content += """
            <div class="section">
                <h2>Current Calibration Coefficients</h2>
                <table class="table">
                    <tr><th>PGA</th><th>Imax</th><th>Offset Register</th><th>Gain Register</th><th>Avg Error (&micro;A)</th></tr>
            """
            # Get current active current PGA setting
            active_current_pga = self.device_info.get('current_pga', None)
            try:
                active_current_pga = int(active_current_pga) if active_current_pga else None
            except (ValueError, TypeError):
                active_current_pga = None
            
            for pga in sorted(self.current_coefficients.keys()):
                coeff = self.current_coefficients[pga]
                accuracy = self.current_accuracy_data.get(pga, {})
                # avg_ppm = accuracy.get('avg_error_ppm', 0)
                avg_delta_uA = accuracy.get('avg_delta_uA', 0)
                
                # Make row bold if this is the active PGA setting
                row_style = 'style="font-weight: bold;"' if pga == active_current_pga else ''
                
                html_content += f"""
                    <tr {row_style}>
                        <td>{pga} - {2**pga}X</td>
                        <td>{coeff['imax']:.6f}</td>
                        <td>{self.format_register_value(coeff['final_offset'])}</td>
                        <td>{self.format_register_value(coeff['final_gain'])}</td>
                        <td>{avg_delta_uA:.2f}</td>
                    </tr>
                """
            html_content += "</table></div>"
        
        if self.dac_coefficients:
            html_content += f"""
            <div class="section">
                <h2>DAC Calibration Coefficients</h2>
                <table class="table">
                    <tr><td><strong>Vmax (V)</strong></td><td>{self.dac_coefficients.get('vmax', 'Unknown'):.4f}</td></tr>
                    <tr><td><strong>Gain Correction</strong></td><td>{self.dac_coefficients.get('final_gain_correction', 'Unknown')}</td></tr>
                    <tr><td><strong>Offset Correction</strong></td><td>{self.dac_coefficients.get('final_offset_correction', 'Unknown')}</td></tr>
                    <tr><td><strong>Average Error (&micro;V)</strong></td><td>{self.dac_accuracy_data.get('avg_delta_uV', 0):.2f}</td></tr>
                </table>
            </div>
            """
        
        html_content += """
        </body>
        </html>
        """
        
        # Write HTML file
        with open(filename, 'w') as f:
            f.write(html_content)
        
        return filename
    
    

    def run_full_calibration_report(self, steps=10, save_data=True, calibrate=True):
        """Run complete calibration and generate report"""
        print("Starting full calibration report generation...")
        
        # Collect device information
        self.collect_device_info()
        
        # Calibrate and collect voltage data for all PGAs
        for pga in range(8):
            self.calibrate_and_collect_voltage_data(pga, steps, save_data=save_data, calibrate=calibrate)
        
        # Calibrate and collect current data for all PGAs  
        for pga in range(8):
            self.calibrate_and_collect_current_data(pga, steps, save_data=save_data, calibrate=calibrate)

        # Calibrate and collect DAC data
        self.calibrate_and_collect_dac_data(steps, save_data=save_data, calibrate=calibrate)

        # Generate HTML report
        html_filename = self.generate_html_report()
    
               
        # Turn off source meter output after calibration is complete
        try:
            instruments.source_meter.write("OUTP OFF")
            print("Source meter output turned off.")
        except Exception as e:
            print(f"Warning: Could not turn off source meter output: {e}")
        
        print("Calibration report complete!")
        return html_filename


def main():
    global amu
    
    parser = argparse.ArgumentParser(description='AMU Calibration Report Generator')
    parser.add_argument('--steps', '-s', type=int, default=10,
                       help='Number of measurement steps for accuracy verification (default: 10)')
    parser.add_argument('--port', type=str,
                       help='Specify COM port (e.g., COM4). If not specified, auto-detect AMU device.')
    parser.add_argument('--output', '-o', type=str,
                       help='Output filename for HTML report')
    parser.add_argument('--property_tag', type=str,
                       help='Property tag for calibration reference (e.g., AET206)')
    parser.add_argument('--nosave', action='store_true',
                       help='Do not save calibration data')
    parser.add_argument('--nocal', action='store_true',
                       help='Do not calibrate the device, only generate report based on existing data and run verification')
    parser.add_argument('--channel', type=int,
                       help='Channel number for passthrough communication (1-based). Commands will use @n SCPI address format.')
    args = parser.parse_args()
    
    # Initialize AMU connection
    print("Initializing AMU connection...")
    amu = AMU(com_port=args.port, channel=args.channel)
    
    if not amu.available:
        if args.port:
            print(f"AMU device not available on specified port {args.port}. Please check connection and port.")
        else:
            print("AMU device not available. Please check connection or specify COM port with --port.")
        return
    
    # Validate channel if specified
    if args.channel is not None:
        if not amu.validate_channel():
            print(f"Channel validation failed. Exiting.")
            amu.disconnect()
            return
    
    amu.send("*CLS")
    
    firmware = amu.query("SYST:FIRM?")
    if args.channel:
        print(f"AMU connected with firmware version {firmware} on channel {args.channel}")
    else:
        print(f"AMU connected with firmware version {firmware}")
    
    # Create calibration report
    report = CalibrationReport(property_tag=args.property_tag)
    
    try:
        html_filename = report.run_full_calibration_report(steps=args.steps, save_data=not args.nosave, calibrate=not args.nocal)

        # Open report in browser
        webbrowser.open('file://' + os.path.realpath(html_filename))

        print(f"HTML report generated: {html_filename}")
        
    except Exception as e:
        print(f"Error during calibration: {e}")
    finally:
        amu.disconnect()


if __name__ == "__main__":
    main()