#!/usr/bin/env python3
"""
Script to extract calibration data from AMU calibration HTML reports.
Extracts plot data and converts to pandas DataFrames for analysis and plotting.
"""

import re
import json
import pandas as pd
from pathlib import Path
import plotly.graph_objects as go
import plotly.subplots as sp
import argparse
from typing import Dict, List, Tuple, Optional


def extract_plotly_data(html_content: str, plot_id: str) -> Optional[Dict]:
    """
    Extract Plotly data from HTML content for a specific plot ID.
    
    Args:
        html_content: HTML content as string
        plot_id: The plot ID to search for (e.g., 'plot_voltage_accuracy')
    
    Returns:
        Dictionary containing the plot data or None if not found
    """
    # Pattern to find the Plotly.newPlot call for the specific plot
    pattern = rf'Plotly\.newPlot\(\s*"{plot_id}",\s*(\[.*?\]),\s*(\{{.*?\}}),\s*(\{{.*?\}})\s*\)'
    
    match = re.search(pattern, html_content, re.DOTALL)
    if not match:
        print(f"Warning: Could not find plot data for {plot_id}")
        return None
    
    try:
        # Extract the data array (first capture group)
        data_str = match.group(1)
        # Clean up the data string and parse as JSON
        data_str = re.sub(r'(?<!\\)"([^"]*)"(?=\s*:)', r'"\1"', data_str)  # Ensure proper JSON quotes
        data = json.loads(data_str)
        return data
    except json.JSONDecodeError as e:
        print(f"Error parsing JSON for {plot_id}: {e}")
        return None


def plotly_to_dataframe(plot_data: List[Dict], plot_type: str) -> pd.DataFrame:
    """
    Convert Plotly data to pandas DataFrame.
    
    Args:
        plot_data: List of plotly trace dictionaries
        plot_type: Type of plot ('voltage', 'current', or 'dac')
    
    Returns:
        pandas DataFrame with the plot data
    """
    all_data = []
    
    for trace in plot_data:
        trace_name = trace.get('name', 'Unknown')
        x_values = trace.get('x', [])
        y_values = trace.get('y', [])
        
        # Create rows for each data point
        for x, y in zip(x_values, y_values):
            row = {
                'trace_name': trace_name,
                'x_value': x,
                'y_value': y,
                'plot_type': plot_type
            }
            
            # Add specific column names based on plot type
            if plot_type == 'voltage':
                row['normalized_full_scale'] = x
                row['measurement_error_ppm'] = y
                # Extract PGA info from trace name
                pga_match = re.search(r'PGA (\d+)', trace_name)
                vmax_match = re.search(r'Vmax=([\d.]+)V', trace_name)
                row['pga'] = int(pga_match.group(1)) if pga_match else None
                row['vmax'] = float(vmax_match.group(1)) if vmax_match else None
                
            elif plot_type == 'current':
                row['normalized_full_scale'] = x
                row['measurement_error_ppm'] = y
                # Extract PGA info from trace name
                pga_match = re.search(r'PGA (\d+)', trace_name)
                imax_match = re.search(r'Imax=([\d.]+)A', trace_name)
                row['pga'] = int(pga_match.group(1)) if pga_match else None
                row['imax'] = float(imax_match.group(1)) if imax_match else None
                
            elif plot_type == 'dac':
                row['voltage_setpoint'] = x
                row['measurement_error_ppm'] = y
                
            all_data.append(row)
    
    return pd.DataFrame(all_data)


def extract_table_data(html_content: str) -> Dict[str, pd.DataFrame]:
    """
    Extract calibration coefficient tables from HTML.
    
    Args:
        html_content: HTML content as string
    
    Returns:
        Dictionary of DataFrames for each calibration table
    """
    tables = {}
    
    # Extract voltage calibration table
    voltage_pattern = r'<h2>Voltage Calibration Coefficients</h2>.*?<table class="table">(.*?)</table>'
    voltage_match = re.search(voltage_pattern, html_content, re.DOTALL)
    if voltage_match:
        voltage_table_html = voltage_match.group(1)
        # Parse table rows
        row_pattern = r'<tr[^>]*>.*?<td>(\d+)</td>.*?<td>([\d.]+)</td>.*?<td>(0x[A-F0-9]+ \(\d+\))</td>.*?<td>(0x[A-F0-9]+ \(\d+\))</td>.*?<td>([\d.]+)</td>.*?</tr>'
        rows = re.findall(row_pattern, voltage_table_html, re.DOTALL)
        
        voltage_data = []
        for row in rows:
            pga, vmax, offset_reg, gain_reg, avg_error = row
            voltage_data.append({
                'pga': int(pga),
                'vmax': float(vmax),
                'offset_register': offset_reg.strip(),
                'gain_register': gain_reg.strip(),
                'avg_error_uv': float(avg_error)
            })
        tables['voltage_coefficients'] = pd.DataFrame(voltage_data)
    
    # Extract current calibration table
    current_pattern = r'<h2>Current Calibration Coefficients</h2>.*?<table class="table">(.*?)</table>'
    current_match = re.search(current_pattern, html_content, re.DOTALL)
    if current_match:
        current_table_html = current_match.group(1)
        row_pattern = r'<tr[^>]*>.*?<td>(\d+)</td>.*?<td>([\d.]+)</td>.*?<td>(0x[A-F0-9]+ \(\d+\))</td>.*?<td>(0x[A-F0-9]+ \(\d+\))</td>.*?<td>([\d.]+)</td>.*?</tr>'
        rows = re.findall(row_pattern, current_table_html, re.DOTALL)
        
        current_data = []
        for row in rows:
            pga, imax, offset_reg, gain_reg, avg_error = row
            current_data.append({
                'pga': int(pga),
                'imax': float(imax),
                'offset_register': offset_reg.strip(),
                'gain_register': gain_reg.strip(),
                'avg_error_ua': float(avg_error)
            })
        tables['current_coefficients'] = pd.DataFrame(current_data)
    
    # Extract DAC calibration table
    dac_pattern = r'<h2>DAC Calibration Coefficients</h2>.*?<table class="table">(.*?)</table>'
    dac_match = re.search(dac_pattern, html_content, re.DOTALL)
    if dac_match:
        dac_table_html = dac_match.group(1)
        # Extract individual values
        vmax_match = re.search(r'<td><strong>Vmax</strong></td><td>([\d.]+)</td>', dac_table_html)
        gain_match = re.search(r'<td><strong>Gain Correction</strong></td><td>([\d.]+)</td>', dac_table_html)
        offset_match = re.search(r'<td><strong>Offset Correction</strong></td><td>([\d.]+)</td>', dac_table_html)
        error_match = re.search(r'<td><strong>Average Error.*?</strong></td><td>([\d.]+)</td>', dac_table_html)
        
        dac_data = [{
            'vmax': float(vmax_match.group(1)) if vmax_match else None,
            'gain_correction': float(gain_match.group(1)) if gain_match else None,
            'offset_correction': float(offset_match.group(1)) if offset_match else None,
            'avg_error_uv': float(error_match.group(1)) if error_match else None
        }]
        tables['dac_coefficients'] = pd.DataFrame(dac_data)
    
    return tables


def extract_device_info(html_content: str) -> Dict[str, str]:
    """Extract device information from HTML."""
    info = {}
    
    # Extract basic device info
    patterns = {
        'generated': r'<p><strong>Generated:</strong> ([^<]+)</p>',
        'device': r'<p><strong>Device:</strong> ([^<]+)</p>',
        'serial_number': r'<p><strong>Serial Number:</strong> ([^<]+)</p>',
        'calibration_reference': r'<p><strong>Calibration Reference:</strong> ([^<]+)</p>',
        'temperature': r'<td><strong>Temperature</strong></td><td>([^<]+)</td>',
        'firmware': r'<td><strong>Firmware</strong></td><td>([^<]+)</td>'
    }
    
    for key, pattern in patterns.items():
        match = re.search(pattern, html_content)
        if match:
            info[key] = match.group(1).strip()
    
    return info


def plot_calibration_data(voltage_df: pd.DataFrame, current_df: pd.DataFrame, dac_df: pd.DataFrame, 
                         output_dir: Path = None):
    """
    Create plots from the extracted calibration data using Plotly.
    
    Args:
        voltage_df: Voltage calibration DataFrame
        current_df: Current calibration DataFrame  
        dac_df: DAC calibration DataFrame
        output_dir: Directory to save plots (optional)
    """
    # Create subplots
    fig = sp.make_subplots(
        rows=2, cols=2,
        subplot_titles=('ADC Voltage Calibration Accuracy', 'ADC Current Calibration Accuracy', 
                       'DAC Calibration Accuracy', 'Summary Statistics'),
        specs=[[{"secondary_y": False}, {"secondary_y": False}],
               [{"secondary_y": False}, {"type": "table"}]]
    )
    
    # Voltage accuracy plot
    for pga in voltage_df['pga'].unique():
        pga_data = voltage_df[voltage_df['pga'] == pga]
        vmax = pga_data['vmax'].iloc[0]
        fig.add_trace(
            go.Scatter(
                x=pga_data['normalized_full_scale'], 
                y=pga_data['measurement_error_ppm'],
                mode='lines+markers',
                name=f'PGA {pga} (Vmax={vmax}V)',
                line=dict(width=2),
                marker=dict(size=6)
            ),
            row=1, col=1
        )
    
    # Current accuracy plot
    for pga in current_df['pga'].unique():
        pga_data = current_df[current_df['pga'] == pga]
        imax = pga_data['imax'].iloc[0]
        fig.add_trace(
            go.Scatter(
                x=pga_data['normalized_full_scale'], 
                y=pga_data['measurement_error_ppm'],
                mode='lines+markers',
                name=f'PGA {pga} (Imax={imax}A)',
                line=dict(width=2),
                marker=dict(size=6),
                showlegend=False  # Avoid legend duplication
            ),
            row=1, col=2
        )
    
    # DAC accuracy plot
    fig.add_trace(
        go.Scatter(
            x=dac_df['voltage_setpoint'], 
            y=dac_df['measurement_error_ppm'],
            mode='lines+markers',
            name='DAC Accuracy',
            line=dict(color='red', width=2),
            marker=dict(color='red', size=6),
            showlegend=False
        ),
        row=2, col=1
    )
    
    # Calculate summary statistics
    voltage_rms = (voltage_df['measurement_error_ppm']**2).mean()**0.5
    current_rms = (current_df['measurement_error_ppm']**2).mean()**0.5
    dac_rms = (dac_df['measurement_error_ppm']**2).mean()**0.5
    
    # Summary statistics table
    summary_data = [
        ['Metric', 'Value'],
        ['Voltage RMS Error (ppm)', f'{voltage_rms:.1f}'],
        ['Current RMS Error (ppm)', f'{current_rms:.1f}'],
        ['DAC RMS Error (ppm)', f'{dac_rms:.1f}'],
        ['Voltage PGA Count', str(voltage_df['pga'].nunique())],
        ['Current PGA Count', str(current_df['pga'].nunique())],
        ['Voltage Data Points', str(len(voltage_df))],
        ['Current Data Points', str(len(current_df))],
        ['DAC Data Points', str(len(dac_df))]
    ]
    
    fig.add_trace(
        go.Table(
            header=dict(values=summary_data[0], fill_color='lightblue'),
            cells=dict(values=list(zip(*summary_data[1:])), fill_color='white')
        ),
        row=2, col=2
    )
    
    # Update layout
    fig.update_xaxes(title_text="Normalized Full Scale (0-1)", row=1, col=1)
    fig.update_yaxes(title_text="Measurement Error (ppm)", row=1, col=1)
    fig.update_xaxes(title_text="Normalized Full Scale (0-1)", row=1, col=2)
    fig.update_yaxes(title_text="Measurement Error (ppm)", row=1, col=2)
    fig.update_xaxes(title_text="Voltage Setpoint (V)", row=2, col=1)
    fig.update_yaxes(title_text="Measurement Error (ppm)", row=2, col=1)
    
    fig.update_layout(
        height=800,
        title_text="AMU Calibration Data Analysis",
        showlegend=True
    )
    
    if output_dir:
        html_path = output_dir / 'calibration_analysis.html'
        fig.write_html(html_path)
        print(f"Plot saved to {html_path}")
    
    fig.show()


def main():
    parser = argparse.ArgumentParser(description='Extract calibration data from AMU HTML reports')
    parser.add_argument('html_file', help='Path to HTML calibration report')
    parser.add_argument('--output-dir', '-o', help='Output directory for CSV files and plots')
    parser.add_argument('--plot', '-p', action='store_true', help='Generate plots')
    parser.add_argument('--csv', '-c', action='store_true', help='Save data as CSV files')
    
    args = parser.parse_args()
    
    # Read HTML file
    html_file = Path(args.html_file)
    if not html_file.exists():
        print(f"Error: File {html_file} not found")
        return
    
    # Try different encodings
    encodings = ['utf-8', 'latin-1', 'cp1252', 'iso-8859-1']
    html_content = None
    
    for encoding in encodings:
        try:
            with open(html_file, 'r', encoding=encoding) as f:
                html_content = f.read()
            print(f"Successfully read file with {encoding} encoding")
            break
        except UnicodeDecodeError:
            continue
    
    if html_content is None:
        print("Error: Could not read file with any supported encoding")
        return
    
    # Set up output directory
    output_dir = Path(args.output_dir) if args.output_dir else html_file.parent
    output_dir.mkdir(exist_ok=True)
    
    print(f"Processing calibration report: {html_file.name}")
    
    # Extract device information
    device_info = extract_device_info(html_content)
    print(f"Device: {device_info.get('device', 'Unknown')}")
    print(f"Serial: {device_info.get('serial_number', 'Unknown')}")
    print(f"Generated: {device_info.get('generated', 'Unknown')}")
    
    # Extract plot data
    print("\nExtracting plot data...")
    voltage_data = extract_plotly_data(html_content, 'plot_voltage_accuracy')
    current_data = extract_plotly_data(html_content, 'plot_current_accuracy')
    dac_data = extract_plotly_data(html_content, 'plot_dac_accuracy')
    
    # Convert to DataFrames
    dataframes = {}
    if voltage_data:
        dataframes['voltage_plot'] = plotly_to_dataframe(voltage_data, 'voltage')
        print(f"Extracted voltage data: {len(dataframes['voltage_plot'])} points")
    
    if current_data:
        dataframes['current_plot'] = plotly_to_dataframe(current_data, 'current')
        print(f"Extracted current data: {len(dataframes['current_plot'])} points")
    
    if dac_data:
        dataframes['dac_plot'] = plotly_to_dataframe(dac_data, 'dac')
        print(f"Extracted DAC data: {len(dataframes['dac_plot'])} points")
    
    # Extract table data
    print("\nExtracting table data...")
    table_data = extract_table_data(html_content)
    dataframes.update(table_data)
    
    for table_name, df in table_data.items():
        print(f"Extracted {table_name}: {len(df)} rows")
    
    # Save as CSV if requested
    if args.csv:
        print(f"\nSaving CSV files to {output_dir}")
        for name, df in dataframes.items():
            csv_path = output_dir / f"{html_file.stem}_{name}.csv"
            df.to_csv(csv_path, index=False)
            print(f"Saved: {csv_path}")
        
        # Save device info
        device_df = pd.DataFrame([device_info])
        device_csv = output_dir / f"{html_file.stem}_device_info.csv"
        device_df.to_csv(device_csv, index=False)
        print(f"Saved: {device_csv}")
    
    # Generate plots if requested
    if args.plot and all(k in dataframes for k in ['voltage_plot', 'current_plot', 'dac_plot']):
        print("\nGenerating plots...")
        plot_calibration_data(
            dataframes['voltage_plot'], 
            dataframes['current_plot'], 
            dataframes['dac_plot'],
            output_dir
        )
    
    print(f"\nExtraction complete! Data available as:")
    for name, df in dataframes.items():
        print(f"  {name}: {df.shape}")
    
    return dataframes


if __name__ == "__main__":
    main()