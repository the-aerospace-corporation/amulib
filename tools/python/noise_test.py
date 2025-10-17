"""
AMU Noise Test Tool - Enhanced with Statistical Analysis

This script provides measurement capabilities for the AMU (Aerospace Measurement Unit)
with comprehensive statistical analysis. Key features:

- Unified measurement function supporting VOC, ISC, and RTD measurements
- Real-time data streaming to Teleplot
- Statistical analysis (min, max, mean, median, std dev, range)
- Command-line interface with flexible options
- Interactive console mode
- Automatic result saving to timestamped files

Usage examples:
  python noise_test.py --measure voc --points 100    # Run VOC with 100 points
  python noise_test.py --measure isc                 # Run ISC with default 200 points  
  python noise_test.py --port COM4                   # Connect to specific COM port
  python noise_test.py --port COM3 --measure voc     # Connect to COM3 and run VOC
  python noise_test.py                               # Interactive mode (auto-detect)
  python noise_test.py --command "*IDN?"             # Single SCPI command

Author: Colin Mann
Date: October 2025
"""

import sys
import time
import argparse
from datetime import datetime
from amu import amu as AMU
import numpy as np
import teleplot as teleplot

measurement_delay = 0.01

# Global AMU instance - will be initialized in main() with optional COM port
amu = None

def measure_and_analyze(measurement_type, numpoints=200):
    """
    Generic function to perform measurements and statistical analysis
    
    Args:
        measurement_type (str): Type of measurement - 'voc', 'isc', or 'rtd'
        numpoints (int): Number of measurement points to collect
    
    Returns:
        dict: Dictionary containing measurement data and statistics
    """
    # Define measurement commands and parameters
    measurement_config = {
        'voc': {
            'command': "SWEEP:TRIG:VOC?",
            'teleplot_name': 'voc',
            'unit': 'µV',
            'scale_factor': 1000000.0,
            'has_temperature': True,
            'value_name': 'VOC'
        },
        'isc': {
            'command': "SWEEP:TRIG:ISC?",
            'teleplot_name': 'isc', 
            'unit': 'µA',
            'scale_factor': 1000000.0,
            'has_temperature': True,
            'value_name': 'ISC'
        },
        'rtd': {
            'command': "MEAS:ADC:TSENS?",
            'teleplot_name': 'temp',
            'unit': '°C',
            'scale_factor': 1.0,
            'has_temperature': False,
            'value_name': 'TEMP'
        }
    }
    
    if measurement_type not in measurement_config:
        raise ValueError(f"Unknown measurement type: {measurement_type}. Must be one of {list(measurement_config.keys())}")
    
    config = measurement_config[measurement_type]
    
    # Arrays to store measurements
    primary_values = []
    temperature_values = []
    
    print(f"\nStarting {config['value_name']} measurement collection ({numpoints} points)...")
    print("=" * 60)
    
    for i in range(numpoints):
        try:
            # Get measurement from AMU
            measurement = amu.query(config['command'])
            
            if config['has_temperature']:
                # Parse response with temperature (format: "value,temperature")
                parts = measurement.split(',')
                primary_value = float(parts[0]) * config['scale_factor']
                temperature = float(parts[1])
                
                # print(f"{config['value_name']}: {primary_value:.3f} {config['unit']}, TEMP: {temperature:.2f} °C")
                
                # Send to teleplot
                teleplot.sendTelemetry(config['teleplot_name'], primary_value)
                teleplot.sendTelemetry("temp", temperature)
                
                # Store values
                primary_values.append(primary_value)
                temperature_values.append(temperature)
                
            else:
                # Parse response without temperature (just the value)
                primary_value = float(measurement) * config['scale_factor']
                
                # print(f"{config['value_name']}: {primary_value:.3f} {config['unit']}")
                
                # Send to teleplot
                teleplot.sendTelemetry(config['teleplot_name'], primary_value)
                
                # Store values
                primary_values.append(primary_value)
            
            time.sleep(measurement_delay)
            
        except Exception as e:
            print(f"Error during measurement {i+1}: {e}")
            continue
    
    # Calculate statistics using numpy
    primary_array = np.array(primary_values)
    
    # Calculate primary value statistics
    stats = {
        'measurement_type': measurement_type,
        'unit': config['unit'],
        'count': len(primary_values),
        'primary_stats': {
            'min': np.min(primary_array),
            'max': np.max(primary_array),
            'mean': np.mean(primary_array),
            'median': np.median(primary_array),
            'std_dev': np.std(primary_array),
            'range': np.max(primary_array) - np.min(primary_array)
        },
        'raw_data': primary_values
    }
    
    # Calculate temperature statistics if available
    if config['has_temperature'] and temperature_values:
        temp_array = np.array(temperature_values)
        stats['temperature_stats'] = {
            'min': np.min(temp_array),
            'max': np.max(temp_array),
            'mean': np.mean(temp_array),
            'median': np.median(temp_array),
            'std_dev': np.std(temp_array),
            'range': np.max(temp_array) - np.min(temp_array)
        }
        stats['temperature_data'] = temperature_values
    
    # Print statistics
    print("\n" + "=" * 60)
    print(f"{config['value_name']} MEASUREMENT STATISTICS")
    print("=" * 60)
    print(f"Number of samples: {stats['count']}")
    print(f"Primary measurement ({config['unit']}):")
    print(f"  Minimum:     {stats['primary_stats']['min']:.3f}")
    print(f"  Maximum:     {stats['primary_stats']['max']:.3f}")
    print(f"  Mean:        {stats['primary_stats']['mean']:.3f}")
    print(f"  Median:      {stats['primary_stats']['median']:.3f}")
    print(f"  Std Dev:     {stats['primary_stats']['std_dev']:.3f}")
    print(f"  Range:       {stats['primary_stats']['range']:.3f}")
    
    if config['has_temperature']:
        print(f"Temperature (°C):")
        print(f"  Minimum:     {stats['temperature_stats']['min']:.2f}")
        print(f"  Maximum:     {stats['temperature_stats']['max']:.2f}")
        print(f"  Mean:        {stats['temperature_stats']['mean']:.2f}")
        print(f"  Median:      {stats['temperature_stats']['median']:.2f}")
        print(f"  Std Dev:     {stats['temperature_stats']['std_dev']:.3f}")
        print(f"  Range:       {stats['temperature_stats']['range']:.2f}")
    
    print("=" * 60)
    
    return stats

def voc(numpoints=200):
    """Measure open circuit voltage with statistical analysis"""
    return measure_and_analyze('voc', numpoints)

def isc(numpoints=200):
    """Measure short circuit current with statistical analysis"""
    return measure_and_analyze('isc', numpoints)

def rtd(numpoints=200):
    """Measure RTD temperature with statistical analysis"""
    return measure_and_analyze('rtd', numpoints)



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

    # Handle measurement execution from command line
    if args.measure:
        try:
            if args.measure == 'voc':
                stats = voc(args.points)
            elif args.measure == 'isc':
                stats = isc(args.points)
            elif args.measure == 'rtd':
                stats = rtd(args.points)
            
            # Optionally save results to file
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"{args.measure}_measurement_{timestamp}.txt"
            with open(filename, 'w') as f:
                f.write(f"AMU {args.measure.upper()} Measurement Results\n")
                f.write(f"Timestamp: {datetime.now()}\n")
                f.write(f"Firmware: {firmware}\n")
                f.write(f"Points collected: {stats['count']}\n\n")
                
                f.write(f"Primary measurement statistics ({stats['unit']}):\n")
                for key, value in stats['primary_stats'].items():
                    f.write(f"  {key}: {value:.6f}\n")
                
                if 'temperature_stats' in stats:
                    f.write(f"\nTemperature statistics (°C):\n")
                    for key, value in stats['temperature_stats'].items():
                        f.write(f"  {key}: {value:.6f}\n")
                
                f.write(f"\nRaw data:\n")
                for i, value in enumerate(stats['raw_data']):
                    if 'temperature_data' in stats:
                        f.write(f"{i+1}: {value:.6f} {stats['unit']}, {stats['temperature_data'][i]:.3f} °C\n")
                    else:
                        f.write(f"{i+1}: {value:.6f} {stats['unit']}\n")
            
            print(f"\nResults saved to: {filename}")
            
        except Exception as e:
            print(f"Error during measurement: {e}")
        finally:
            amu.disconnect()
        return

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
        print("Non-interactive mode detected. Use --measure to run measurements or --command for single commands.")
        print("Available measurements: voc, isc, rtd")
        print("Example: python noise_test.py --measure voc --points 100")
        amu.disconnect()
        return

    # Interactive mode
    user_input = 0
    print("AMU Noise Test Interactive Console")
    print("Available commands: voc [points], isc [points], rtd [points], help, exit")
    print("You can also send SCPI commands directly (use ? for queries)")
    print("Default measurement points: 200")

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
        
        # Parse command and optional point count
        parts = user_input.split()
        command = parts[0] if parts else ""
        points = int(parts[1]) if len(parts) > 1 and parts[1].isdigit() else 200
        
        if command == 'voc':      
            print(f"Running VOC measurement with {points} points...")
            stats = voc(points)
        elif command == 'isc':      
            print(f"Running ISC measurement with {points} points...")
            stats = isc(points)
        elif command == 'rtd':      
            print(f"Running RTD measurement with {points} points...")
            stats = rtd(points)
        elif command == 'help':
            print("Available commands:")
            print("  voc [points]  - Measure open circuit voltage (default: 200 points)")
            print("  isc [points]  - Measure short circuit current (default: 200 points)")
            print("  rtd [points]  - Measure temperature sensor (default: 200 points)")
            print("  help          - Show this help message")
            print("  exit          - Exit the program")
            print("  Any SCPI command (use ? for queries)")
            print("\nExamples:")
            print("  voc 100       - Run VOC measurement with 100 points")
            print("  isc           - Run ISC measurement with 200 points (default)")
            print("  *IDN?         - Query device identification")
        elif command != 'exit':
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