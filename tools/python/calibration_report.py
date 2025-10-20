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

# Try to import PDF generation libraries
try:
    import plotly.io as pio
    # Check if kaleido is available for static image export
    try:
        import kaleido
        STATIC_EXPORT_AVAILABLE = True
    except ImportError:
        STATIC_EXPORT_AVAILABLE = False
    
    PDF_AVAILABLE = STATIC_EXPORT_AVAILABLE
    
except ImportError:
    PDF_AVAILABLE = False
    STATIC_EXPORT_AVAILABLE = False
    
# Try weasyprint (but don't fail if it's not working)
try:
    from weasyprint import HTML, CSS
    WEASYPRINT_AVAILABLE = True
    PDF_AVAILABLE = True
except (ImportError, OSError) as e:
    WEASYPRINT_AVAILABLE = False
    if "pango" in str(e).lower():
        print("Note: WeasyPrint available but Pango library missing. Using alternative PDF generation.")
    
# Try pdfkit as backup
try:
    import pdfkit
    PDFKIT_AVAILABLE = True
    PDF_AVAILABLE = True
except ImportError:
    PDFKIT_AVAILABLE = False

if not PDF_AVAILABLE:
    print("Warning: PDF generation not available. Install plotly[kaleido] for basic PDF support.")

# Configuration
source_meter_visa = 'USB0::0x0957::0x8D18::MY51140506::0::INSTR'
# volt_meter_visa = 'GPIB0::1::INSTR'  
# current_meter_visa = 'GPIB0::2::INSTR'

instruments = CalibrationInstruments(source_meter_visa)
amu = None

class CalibrationReport:
    """Class to handle calibration data collection and report generation"""
    
    @staticmethod
    def format_register_value(value, digits=4):
        """Format register value as 'Hex (Decimal)' """
        try:
            # Convert to integer (handles string input from AMU queries)
            int_val = int(value)
            return f"0x{int_val:0{digits}X} ({int_val})"
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
                0x33: "AMU-ESP32 - 3.3"
            }
            
            self.device_info = {
                'hardware_version': hardware_version,
                'hardware_name': hardware_names.get(hardware_version, f"Unknown (0x{hardware_version:02X})"),
                'firmware': amu.query("SYST:FIRM?").strip('"'),
                'amu_serial': amu.query("SYST:SER?"),
                'i2c_address': amu.query("SYST:TWI:ADD?"),
                'temperature': amu.query("SYST:TEMP?"),
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
    
    def calibrate_and_collect_voltage_data(self, pga, steps=10):
        """Calibrate voltage for specific PGA and collect accuracy data"""
        print(f"Calibrating voltage PGA {pga}...")
        
        try:
            # Set up for voltage calibration
            amu.query("SWEEP:TRIG:VOC?")
            time.sleep(0.5)
            amu.send("ADC:VOLT:PGA " + str(pga))
            vmax = float(amu.query("ADC:VOLT:MAX?").strip())
            
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
            
            # Save calibration
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
            
            for i in range(steps + 1):
                voltage_setpoint = vstart - (step_size * i)
                instruments.set_voltage(voltage_setpoint)
                time.sleep(0.1)
                
                meter_voltage = instruments.measure_voltage()
                amu_voltage = float(amu.query("MEAS:ADC:VOLT?"))
                
                delta = meter_voltage - amu_voltage  # Keep the sign! Positive = AMU reads low, Negative = AMU reads high
                delta_uV = delta * 1000000
                delta_percent = (delta / vmax * 100) if vmax != 0 else 0  # Keep abs() for percentage calculations
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
            
            self.voltage_accuracy_data[pga] = {
                'pga': pga,
                'vmax': vmax,
                'data': accuracy_data,
                'avg_delta_uV': np.mean([abs(d['delta_uV']) for d in accuracy_data]),  # Use abs for average magnitude
                'avg_error_percent': np.mean([d['delta_percent'] for d in accuracy_data]),
                'avg_error_ppm': np.mean([d['delta_ppm'] for d in accuracy_data])
            }
            
        except Exception as e:
            print(f"Error collecting voltage accuracy data PGA {pga}: {e}")
    
    def calibrate_and_collect_current_data(self, pga, steps=10):
        """Calibrate current for specific PGA and collect accuracy data"""
        print(f"Calibrating current PGA {pga}...")
        
        try:
            # Set up for current calibration
            amu.query("SWEEP:TRIG:ISC?")
            time.sleep(0.5)
            amu.send("ADC:CURR:PGA " + str(pga))
            time.sleep(0.25)
            imax = float(amu.query("ADC:CURR:MAX?"))
            
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
            
            # Save calibration
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
            
            for i in range(steps + 1):
                current_setpoint = istart - (step_size * i)
                instruments.set_current(current_setpoint)
                time.sleep(0.25)
                
                meter_current = instruments.measure_current()
                amu_current = float(amu.query("MEAS:ADC:CURR?"))
                
                delta = meter_current - amu_current  # Keep the sign! Positive = AMU reads low, Negative = AMU reads high
                delta_uA = delta * 1000000
                delta_percent = (delta / imax * 100) if imax != 0 else 0  # Keep abs() for percentage calculations
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
            
            self.current_accuracy_data[pga] = {
                'pga': pga,
                'imax': imax,
                'data': accuracy_data,
                'avg_delta_uA': np.mean([abs(d['delta_uA']) for d in accuracy_data]),  # Use abs for average magnitude
                'avg_error_percent': np.mean([d['delta_percent'] for d in accuracy_data]),
                'avg_error_ppm': np.mean([d['delta_ppm'] for d in accuracy_data])
            }
            
        except Exception as e:
            print(f"Error collecting current accuracy data PGA {pga}: {e}")
    
    def calibrate_and_collect_dac_data(self, steps=10):
        """Calibrate DAC and collect accuracy data"""
        print("Calibrating DAC...")
        
        try:
            # Set up for DAC calibration
            amu.query("SWEEP:TRIG:VOC?")
            amu.send("ADC:VOLT:PGA 0")
            vmax = float(amu.query("ADC:VOLT:MAX?"))
            
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
            
            for i in range(steps):
                voltage_setpoint = vstart - (step_size * i)
                amu.send("DAC:VOLTAGE " + str(voltage_setpoint))
                time.sleep(1)
                
                instruments.measure_voltage()
                amu_voltage = float(amu.query("MEAS:ADC:VOLT?"))
                dac_voltage = float(amu.query("DAC:VOLT?"))
                
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
            
            avg_delta = np.mean([abs(d['delta']) for d in accuracy_data])
            
            self.dac_accuracy_data = {
                'vmax': vmax,
                'data': accuracy_data,
                'avg_delta': avg_delta,
                'avg_delta_uV': avg_delta * 1000000,
                'avg_error_percent': avg_delta / vmax * 100 if vmax != 0 else 0,
                'avg_error_ppm': avg_delta / vmax * 1000000 if vmax != 0 else 0
            }
            
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
                <tr><td><strong>Temperature</strong></td><td>{self.device_info.get('temperature', 'Unknown')} °C</td></tr>
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
                <tr><td><strong>AM0 (A/m²)</strong></td><td>{float(config_values[8]):.6f}</td></tr>
                <tr><td><strong>Area (cm²)</strong></td><td>{float(config_values[9]):.4f}</td></tr>
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
                    <tr><th>PGA</th><th>Vmax</th><th>Offset Register</th><th>Gain Register</th><th>Avg Error (µV)</th></tr>
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
                        <td>{pga}</td>
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
                    <tr><th>PGA</th><th>Imax</th><th>Offset Register</th><th>Gain Register</th><th>Avg Error (µA)</th></tr>
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
                        <td>{pga}</td>
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
                    <tr><td><strong>Vmax</strong></td><td>{self.dac_coefficients.get('vmax', 'Unknown'):.4f}</td></tr>
                    <tr><td><strong>Gain Correction</strong></td><td>{self.dac_coefficients.get('final_gain_correction', 'Unknown')}</td></tr>
                    <tr><td><strong>Offset Correction</strong></td><td>{self.dac_coefficients.get('final_offset_correction', 'Unknown')}</td></tr>
                    <tr><td><strong>Average Error (µV)</strong></td><td>{self.dac_accuracy_data.get('avg_delta_uV', 0):.2f}</td></tr>
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
        
        print(f"HTML report generated: {filename}")
        return filename
    
    def generate_pdf_report(self, html_filename=None, pdf_filename=None):
        """Generate PDF report from HTML or static images"""
        if not PDF_AVAILABLE:
            print("PDF generation not available. Install plotly[kaleido] for basic PDF support.")
            return None
            
        if html_filename is None:
            html_filename = self.generate_html_report()
            
        if pdf_filename is None:
            pdf_filename = html_filename.replace('.html', '.pdf')
        
        try:
            # Method 1: Try weasyprint (if Pango is available)
            if WEASYPRINT_AVAILABLE:
                try:
                    HTML(filename=html_filename).write_pdf(pdf_filename)
                    print(f"PDF report generated using weasyprint: {pdf_filename}")
                    return pdf_filename
                except Exception as e:
                    print(f"Weasyprint failed: {e}")
            
            # Method 2: Try pdfkit (requires wkhtmltopdf)
            if PDFKIT_AVAILABLE:
                try:
                    pdfkit.from_file(html_filename, pdf_filename)
                    print(f"PDF report generated using pdfkit: {pdf_filename}")
                    return pdf_filename
                except Exception as e:
                    print(f"pdfkit failed: {e}")
            
            # Method 3: Generate static image-based PDF using matplotlib + reportlab
            if STATIC_EXPORT_AVAILABLE:
                return self.generate_static_pdf_report(pdf_filename)
            
        except Exception as e:
            print(f"Error generating PDF: {e}")
            return None
    
    def generate_static_pdf_report(self, pdf_filename):
        """Generate PDF using static images - simpler approach"""
        try:
            from reportlab.lib.pagesizes import letter, A4
            from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, Image, Table, TableStyle
            from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
            from reportlab.lib.units import inch
            from reportlab.lib import colors
            import io
            import base64
            
            # Create PDF document
            doc = SimpleDocTemplate(pdf_filename, pagesize=A4)
            styles = getSampleStyleSheet()
            story = []
            
            # Title
            title_style = ParagraphStyle('CustomTitle', parent=styles['Heading1'], fontSize=18, spaceAfter=30)
            story.append(Paragraph("AMU Calibration Report", title_style))
            story.append(Spacer(1, 20))
            
            # Device info table
            device_data = [
                ['Parameter', 'Value'],
                ['Hardware', self.device_info.get('hardware_name', 'Unknown')],
                ['Firmware', self.device_info.get('firmware', 'Unknown')],
                ['AMU Serial', self.device_info.get('amu_serial', 'Unknown')],
                ['Calibration Reference', self.device_info.get('calibration_reference', 'Unknown')],
                ['I2C Address', self.format_register_value(str(self.device_info.get('i2c_address', 'Unknown')).strip(), digits=2)],
                ['Temperature', f"{self.device_info.get('temperature', 'Unknown')} °C"],
            ]
            
            device_table = Table(device_data)
            device_table.setStyle(TableStyle([
                ('BACKGROUND', (0, 0), (-1, 0), colors.grey),
                ('TEXTCOLOR', (0, 0), (-1, 0), colors.whitesmoke),
                ('ALIGN', (0, 0), (-1, -1), 'CENTER'),
                ('FONTNAME', (0, 0), (-1, 0), 'Helvetica-Bold'),
                ('FONTSIZE', (0, 0), (-1, 0), 14),
                ('BOTTOMPADDING', (0, 0), (-1, 0), 12),
                ('BACKGROUND', (0, 1), (-1, -1), colors.beige),
                ('GRID', (0, 0), (-1, -1), 1, colors.black)
            ]))
            
            story.append(device_table)
            story.append(Spacer(1, 30))
            
            # Add AMU Configuration table
            story.append(Paragraph("<b>AMU Configuration</b>", styles['Heading2']))
            
            # Parse SWEEP:CONFIG? response for PDF
            config_data = [['Parameter', 'Value']]
            
            try:
                sweep_config_raw = self.device_info.get('sweep_config', '')
                if sweep_config_raw:
                    config_values = sweep_config_raw.strip().split(',')
                    
                    if len(config_values) >= 10:
                        config_data.extend([
                            ['Sweep Type', self.get_sweep_type_name(config_values[0])],
                            ['Number of Points', config_values[1]],
                            ['Delay (ms)', config_values[2]],
                            ['Ratio', config_values[3]],
                            ['Power Setting', config_values[4]],
                            ['DAC Gain', config_values[5]],
                            ['Sweep Averages', config_values[6]],
                            ['ADC Averages', config_values[7]],
                            ['AM0 (A/m²)', f"{float(config_values[8]):.6f}"],
                            ['Area (cm²)', f"{float(config_values[9]):.4f}"]
                        ])
                    else:
                        config_data.append(['Raw Configuration', sweep_config_raw])
                else:
                    config_data.append(['Configuration', 'Not available'])
            except Exception as e:
                config_data.append(['Configuration Error', f"Could not parse: {str(e)}"])
            
            # Add PGA settings to PDF
            try:
                voltage_pga = self.device_info.get('voltage_pga', 'Unknown')
                voltage_max = self.device_info.get('voltage_max', 'Unknown')
                current_pga = self.device_info.get('current_pga', 'Unknown')
                current_max = self.device_info.get('current_max', 'Unknown')
                
                config_data.extend([
                    ['Voltage PGA Setting', f"PGA {voltage_pga} (Max: {float(voltage_max):.4f} V)"],
                    ['Current PGA Setting', f"PGA {current_pga} (Max: {float(current_max):.6f} A)"]
                ])
            except Exception as e:
                config_data.append(['PGA Settings Error', f"Could not parse: {str(e)}"])
            
            config_table = Table(config_data)
            config_table.setStyle(TableStyle([
                ('BACKGROUND', (0, 0), (-1, 0), colors.grey),
                ('TEXTCOLOR', (0, 0), (-1, 0), colors.whitesmoke),
                ('ALIGN', (0, 0), (-1, -1), 'CENTER'),
                ('FONTNAME', (0, 0), (-1, 0), 'Helvetica-Bold'),
                ('FONTSIZE', (0, 0), (-1, 0), 14),
                ('BOTTOMPADDING', (0, 0), (-1, 0), 12),
                ('BACKGROUND', (0, 1), (-1, -1), colors.beige),
                ('GRID', (0, 0), (-1, -1), 1, colors.black)
            ]))
            
            story.append(config_table)
            story.append(Spacer(1, 30))
            
            # Export plots as images and add to PDF
            plots = self.generate_plots()
            
            for plot_name, fig in plots.items():
                try:
                    # Convert plotly figure to image
                    img_bytes = fig.to_image(format="png", width=800, height=600)
                    
                    # Create image object for reportlab
                    img_buffer = io.BytesIO(img_bytes)
                    img = Image(img_buffer, width=6*inch, height=4.5*inch)
                    
                    # Add plot title and image
                    story.append(Paragraph(f"<b>{plot_name.replace('_', ' ').title()}</b>", styles['Heading2']))
                    story.append(Spacer(1, 12))
                    story.append(img)
                    story.append(Spacer(1, 20))
                    
                except Exception as e:
                    print(f"Could not add plot {plot_name}: {e}")
            
            # Add coefficient tables
            if self.voltage_coefficients:
                story.append(Paragraph("<b>Voltage Calibration Coefficients</b>", styles['Heading2']))
                voltage_data = [['PGA', 'Vmax', 'Offset Register', 'Gain Register', 'Avg Error (µV)']]
                
                # Get current active voltage PGA setting
                active_voltage_pga = self.device_info.get('voltage_pga', None)
                try:
                    active_voltage_pga = int(active_voltage_pga) if active_voltage_pga else None
                except (ValueError, TypeError):
                    active_voltage_pga = None
                
                active_voltage_row = None  # Track which row is active for styling
                
                for pga in sorted(self.voltage_coefficients.keys()):
                    coeff = self.voltage_coefficients[pga]
                    accuracy = self.voltage_accuracy_data.get(pga, {})
                    avg_delta_uV = accuracy.get('avg_delta_uV', 0)
                    voltage_data.append([
                        str(pga),
                        f"{coeff['vmax']:.4f}",
                        self.format_register_value(coeff['final_offset']),
                        self.format_register_value(coeff['final_gain']),
                        f"{avg_delta_uV:.2f}"
                    ])
                    
                    # Track the active row index (add 1 for header row)
                    if pga == active_voltage_pga:
                        active_voltage_row = len(voltage_data) - 1
                
                voltage_table = Table(voltage_data)
                voltage_table_style = [
                    ('BACKGROUND', (0, 0), (-1, 0), colors.grey),
                    ('TEXTCOLOR', (0, 0), (-1, 0), colors.whitesmoke),
                    ('ALIGN', (0, 0), (-1, -1), 'CENTER'),
                    ('FONTNAME', (0, 0), (-1, 0), 'Helvetica-Bold'),
                    ('FONTSIZE', (0, 0), (-1, 0), 10),
                    ('BOTTOMPADDING', (0, 0), (-1, 0), 12),
                    ('BACKGROUND', (0, 1), (-1, -1), colors.beige),
                    ('GRID', (0, 0), (-1, -1), 1, colors.black)
                ]
                
                # Add bold formatting for active voltage PGA row
                if active_voltage_row is not None:
                    voltage_table_style.append(('FONTNAME', (0, active_voltage_row), (-1, active_voltage_row), 'Helvetica-Bold'))
                    voltage_table_style.append(('FONTSIZE', (0, active_voltage_row), (-1, active_voltage_row), 12))
                
                voltage_table.setStyle(TableStyle(voltage_table_style))
                
                story.append(voltage_table)
                story.append(Spacer(1, 20))
            
            # Add current coefficient tables
            if self.current_coefficients:
                story.append(Paragraph("<b>Current Calibration Coefficients</b>", styles['Heading2']))
                current_data = [['PGA', 'Imax', 'Offset Register', 'Gain Register', 'Avg Error (µA)']]
                
                # Get current active current PGA setting
                active_current_pga = self.device_info.get('current_pga', None)
                try:
                    active_current_pga = int(active_current_pga) if active_current_pga else None
                except (ValueError, TypeError):
                    active_current_pga = None
                
                active_current_row = None  # Track which row is active for styling

                for pga in sorted(self.current_coefficients.keys()):
                    coeff = self.current_coefficients[pga]
                    accuracy = self.current_accuracy_data.get(pga, {})
                    avg_delta_uA = accuracy.get('avg_delta_uA', 0)
                    current_data.append([
                        str(pga),
                        f"{coeff['imax']:.6f}",
                        self.format_register_value(coeff['final_offset']),
                        self.format_register_value(coeff['final_gain']),
                        f"{avg_delta_uA:.2f}"
                    ])
                    
                    # Track the active row index (add 1 for header row)
                    if pga == active_current_pga:
                        active_current_row = len(current_data) - 1
                
                current_table = Table(current_data)
                current_table_style = [
                    ('BACKGROUND', (0, 0), (-1, 0), colors.grey),
                    ('TEXTCOLOR', (0, 0), (-1, 0), colors.whitesmoke),
                    ('ALIGN', (0, 0), (-1, -1), 'CENTER'),
                    ('FONTNAME', (0, 0), (-1, 0), 'Helvetica-Bold'),
                    ('FONTSIZE', (0, 0), (-1, 0), 10),
                    ('BOTTOMPADDING', (0, 0), (-1, 0), 12),
                    ('BACKGROUND', (0, 1), (-1, -1), colors.beige),
                    ('GRID', (0, 0), (-1, -1), 1, colors.black)
                ]
                
                # Add bold formatting for active current PGA row
                if active_current_row is not None:
                    current_table_style.append(('FONTNAME', (0, active_current_row), (-1, active_current_row), 'Helvetica-Bold'))
                    current_table_style.append(('FONTSIZE', (0, active_current_row), (-1, active_current_row), 12))
                
                current_table.setStyle(TableStyle(current_table_style))
                
                story.append(current_table)
                story.append(Spacer(1, 20))
            
            # Build PDF
            doc.build(story)
            print(f"Static PDF report generated using reportlab: {pdf_filename}")
            return pdf_filename
            
        except ImportError:
            print("reportlab not available. Install with: pip install reportlab")
            return None
        except Exception as e:
            print(f"Error generating static PDF: {e}")
            return None
    
    def run_full_calibration_report(self, steps=10):
        """Run complete calibration and generate report"""
        print("Starting full calibration report generation...")
        
        # Collect device information
        self.collect_device_info()
        
        # Calibrate and collect voltage data for all PGAs
        for pga in range(8):
            self.calibrate_and_collect_voltage_data(pga, steps)
        
        # Calibrate and collect current data for all PGAs  
        for pga in range(8):
            self.calibrate_and_collect_current_data(pga, steps)
        
        # Calibrate and collect DAC data
        self.calibrate_and_collect_dac_data(steps)
        
        # Generate HTML report
        html_filename = self.generate_html_report()
        
        # Also generate PDF if available
        pdf_filename = self.generate_pdf_report(html_filename)
        
        # Turn off source meter output after calibration is complete
        try:
            instruments.source_meter.write("OUTP OFF")
            print("Source meter output turned off.")
        except Exception as e:
            print(f"Warning: Could not turn off source meter output: {e}")
        
        print("Calibration report complete!")
        return html_filename, pdf_filename


def main():
    global amu
    
    parser = argparse.ArgumentParser(description='AMU Calibration Report Generator')
    parser.add_argument('--steps', '-s', type=int, default=10,
                       help='Number of measurement steps for accuracy verification (default: 10)')
    parser.add_argument('--port', type=str,
                       help='Specify COM port (e.g., COM4). If not specified, auto-detect AMU device.')
    parser.add_argument('--output', '-o', type=str,
                       help='Output filename for HTML report')
    parser.add_argument('--pdf', action='store_true',
                       help='Generate PDF report (requires weasyprint or wkhtmltopdf)')
    parser.add_argument('--property_tag', type=str,
                       help='Property tag for calibration reference (e.g., AET206)')
    args = parser.parse_args()
    
    # Initialize AMU connection
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
    
    # Create calibration report
    report = CalibrationReport(property_tag=args.property_tag)
    
    try:
        html_filename, pdf_filename = report.run_full_calibration_report(steps=args.steps)
        
        # Open report in browser
        webbrowser.open('file://' + os.path.realpath(html_filename))
        
        if pdf_filename:
            print(f"Both HTML and PDF reports generated:")
            print(f"  HTML: {html_filename}")
            print(f"  PDF:  {pdf_filename}")
        else:
            print(f"HTML report generated: {html_filename}")
        
    except Exception as e:
        print(f"Error during calibration: {e}")
    finally:
        amu.disconnect()


if __name__ == "__main__":
    main()