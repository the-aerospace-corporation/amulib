#!/usr/bin/env python3
"""
Script to compare pre-bake and post-bake calibration data.
Plots voltage accuracy for PGA 0, current accuracy for PGA 2, and DAC accuracy.
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
        'firmware': r'<td><strong>Firmware</strong></td><td>([^<]+)</td>',
        'notes': r'<td><strong>Notes</strong></td><td>([^<]+)</td>'
    }
    
    for key, pattern in patterns.items():
        match = re.search(pattern, html_content)
        if match:
            info[key] = match.group(1).strip()
    
    return info


def load_calibration_file(filepath: str, label: str) -> Dict[str, pd.DataFrame]:
    """
    Load calibration data from HTML file.
    
    Args:
        filepath: Path to HTML calibration report
        label: Label for this dataset (e.g., 'pre-bake', 'post-bake')
    
    Returns:
        Dictionary containing DataFrames with calibration data
    """
    html_file = Path(filepath)
    if not html_file.exists():
        print(f"Error: File {html_file} not found")
        return {}
    
    # Try different encodings
    encodings = ['utf-8', 'latin-1', 'cp1252', 'iso-8859-1']
    html_content = None
    
    for encoding in encodings:
        try:
            with open(html_file, 'r', encoding=encoding) as f:
                html_content = f.read()
            print(f"Successfully read {label} file with {encoding} encoding")
            break
        except UnicodeDecodeError:
            continue
    
    if html_content is None:
        print(f"Error: Could not read {label} file with any supported encoding")
        return {}
    
    # Extract device information
    device_info = extract_device_info(html_content)
    
    # Extract plot data
    voltage_data = extract_plotly_data(html_content, 'plot_voltage_accuracy')
    current_data = extract_plotly_data(html_content, 'plot_current_accuracy')
    dac_data = extract_plotly_data(html_content, 'plot_dac_accuracy')
    
    # Convert to DataFrames
    data = {}
    if voltage_data:
        data['voltage'] = plotly_to_dataframe(voltage_data, 'voltage')
        data['voltage']['dataset'] = label
    
    if current_data:
        data['current'] = plotly_to_dataframe(current_data, 'current')
        data['current']['dataset'] = label
    
    if dac_data:
        data['dac'] = plotly_to_dataframe(dac_data, 'dac')
        data['dac']['dataset'] = label
    
    data['device_info'] = pd.DataFrame([device_info])
    data['device_info']['dataset'] = label
    
    return data


def create_comparison_plot(pre_bake_data: Dict, post_bake_data: Dict) -> go.Figure:
    """
    Create comparison plots for pre-bake vs post-bake calibration data.
    
    Args:
        pre_bake_data: Pre-bake calibration data
        post_bake_data: Post-bake calibration data
    
    Returns:
        Plotly figure with comparison plots
    """
    # Generate title from pre-bake data
    title_text = "Pre-Bake vs Post-Bake Calibration Comparison"  # Default
    if 'device_info' in pre_bake_data and not pre_bake_data['device_info'].empty:
        device_info = pre_bake_data['device_info'].iloc[0]
        notes = device_info.get('notes', '')
        serial = device_info.get('serial_number', '')
        if notes and serial:
            title_text = f"{notes} ({serial})"
        elif serial:
            title_text = f"AMU Calibration Report ({serial})"
    
    # Create subplots with more space for legends
    fig = sp.make_subplots(
        rows=2, cols=3,
        subplot_titles=(
            'Voltage Accuracy - PGA 0 (Pre vs Post Bake)',
            'Current Accuracy - PGA 2 (Pre vs Post Bake)',
            'DAC Accuracy (Pre vs Post Bake)',
            '', '', 'Summary Statistics'  # Empty titles for first 2 columns in row 2
        ),
        specs=[[{"secondary_y": False}, {"secondary_y": False}, {"secondary_y": False}],
               [{"colspan": 3, "type": "table"}, None, None]],  # Span table across all 3 columns
        horizontal_spacing=0.08,  # Adjust spacing for 3 columns
        vertical_spacing=0.15
    )
    
    # Colors for pre/post bake
    colors = {'pre-bake': '#1f77b4', 'post-bake': '#ff7f0e'}
    
    # 1. Voltage Accuracy - PGA 0
    for dataset_name, data in [('pre-bake', pre_bake_data), ('post-bake', post_bake_data)]:
        if 'voltage' in data:
            voltage_df = data['voltage']
            pga0_data = voltage_df[voltage_df['pga'] == 0]
            if not pga0_data.empty:
                vmax = pga0_data['vmax'].iloc[0]
                fig.add_trace(
                    go.Scatter(
                        x=pga0_data['normalized_full_scale'],
                        y=pga0_data['measurement_error_ppm'],
                        mode='lines+markers',
                        name=f'{dataset_name.title()} (Vmax={vmax}V)',
                        line=dict(color=colors[dataset_name], width=2),
                        marker=dict(color=colors[dataset_name], size=6),
                        legendgroup='voltage',
                        legendgrouptitle_text="Voltage PGA 0"
                    ),
                    row=1, col=1
                )
    
    # 2. Current Accuracy - PGA 2
    for dataset_name, data in [('pre-bake', pre_bake_data), ('post-bake', post_bake_data)]:
        if 'current' in data:
            current_df = data['current']
            pga2_data = current_df[current_df['pga'] == 2]
            if not pga2_data.empty:
                imax = pga2_data['imax'].iloc[0]
                fig.add_trace(
                    go.Scatter(
                        x=pga2_data['normalized_full_scale'],
                        y=pga2_data['measurement_error_ppm'],
                        mode='lines+markers',
                        name=f'{dataset_name.title()} (Imax={imax}A)',
                        line=dict(color=colors[dataset_name], width=2),
                        marker=dict(color=colors[dataset_name], size=6),
                        legendgroup='current',
                        legendgrouptitle_text="Current PGA 2"
                    ),
                    row=1, col=2
                )
    
    # 3. DAC Accuracy
    for dataset_name, data in [('pre-bake', pre_bake_data), ('post-bake', post_bake_data)]:
        if 'dac' in data:
            dac_df = data['dac']
            fig.add_trace(
                go.Scatter(
                    x=dac_df['voltage_setpoint'],
                    y=dac_df['measurement_error_ppm'],
                    mode='lines+markers',
                    name=f'{dataset_name.title()} DAC',
                    line=dict(color=colors[dataset_name], width=2),
                    marker=dict(color=colors[dataset_name], size=6),
                    legendgroup='dac',
                    legendgrouptitle_text="DAC Accuracy"
                ),
                row=1, col=3
            )
    
    # 4. Summary Statistics Table
    summary_data = [['Metric', 'Pre-Bake', 'Post-Bake', 'Change']]
    
    # Calculate statistics for each dataset
    stats = {}
    for dataset_name, data in [('pre-bake', pre_bake_data), ('post-bake', post_bake_data)]:
        stats[dataset_name] = {}
        
        # Device info
        if 'device_info' in data and not data['device_info'].empty:
            device_info = data['device_info'].iloc[0]
            stats[dataset_name]['temperature'] = device_info.get('temperature', 'N/A')
            stats[dataset_name]['generated'] = device_info.get('generated', 'N/A')
        
        # Voltage PGA 0 RMS error
        if 'voltage' in data:
            voltage_df = data['voltage']
            pga0_data = voltage_df[voltage_df['pga'] == 0]
            if not pga0_data.empty:
                stats[dataset_name]['voltage_rms'] = (pga0_data['measurement_error_ppm']**2).mean()**0.5
        
        # Current PGA 2 RMS error
        if 'current' in data:
            current_df = data['current']
            pga2_data = current_df[current_df['pga'] == 2]
            if not pga2_data.empty:
                stats[dataset_name]['current_rms'] = (pga2_data['measurement_error_ppm']**2).mean()**0.5
        
        # DAC RMS error
        if 'dac' in data:
            dac_df = data['dac']
            stats[dataset_name]['dac_rms'] = (dac_df['measurement_error_ppm']**2).mean()**0.5
    
    # Build summary table
    metrics = [
        ('Voltage RMS (ppm)', 'voltage_rms', 'ppm'),
        ('Current RMS (ppm)', 'current_rms', 'ppm'),
        ('DAC RMS (ppm)', 'dac_rms', 'ppm')
    ]
    
    for metric_name, key, unit in metrics:
        pre_val = stats.get('pre-bake', {}).get(key, 'N/A')
        post_val = stats.get('post-bake', {}).get(key, 'N/A')
        
        # Calculate change for numeric values
        change = 'N/A'
        if isinstance(pre_val, (int, float)) and isinstance(post_val, (int, float)):
            change = f"{post_val - pre_val:+.1f}"
            pre_val = f"{pre_val:.1f}"
            post_val = f"{post_val:.1f}"
        
        summary_data.append([metric_name, str(pre_val), str(post_val), str(change)])
    
    fig.add_trace(
        go.Table(
            header=dict(values=summary_data[0], fill_color='lightblue', font=dict(size=12)),
            cells=dict(values=list(zip(*summary_data[1:])), fill_color='white', font=dict(size=11))
        ),
        row=2, col=1
    )
    
    # Update layout
    fig.update_xaxes(title_text="Normalized Full Scale (0-1)", row=1, col=1)
    fig.update_yaxes(title_text="Measurement Error (ppm)", row=1, col=1, range=[-500, 500])
    fig.update_xaxes(title_text="Normalized Full Scale (0-1)", row=1, col=2)
    fig.update_yaxes(title_text="Measurement Error (ppm)", row=1, col=2, range=[-500, 500])
    fig.update_xaxes(title_text="Voltage Setpoint (V)", row=1, col=3)
    fig.update_yaxes(title_text="Measurement Error (ppm)", row=1, col=3, range=[-20, 20])
    
    fig.update_layout(
        height=800,
        width=1600,  # Wider to accommodate 3 plots in a row
        title_text=title_text,
        showlegend=True,
        legend=dict(
            orientation="v",
            yanchor="top",
            y=1,
            xanchor="left", 
            x=1.02,  # Position legend to the right of the plots
            groupclick="toggleitem"
        ),
        margin=dict(r=200)  # Right margin for legend space
    )
    
    return fig


def print_comparison_summary(pre_bake_data: Dict, post_bake_data: Dict):
    """Print detailed comparison summary."""
    print("=" * 80)
    print("PRE-BAKE vs POST-BAKE CALIBRATION COMPARISON")
    print("=" * 80)
    
    # Device information
    for dataset_name, data in [('PRE-BAKE', pre_bake_data), ('POST-BAKE', post_bake_data)]:
        if 'device_info' in data and not data['device_info'].empty:
            device_info = data['device_info'].iloc[0]
            print(f"\n{dataset_name} Device Info:")
            print(f"  Serial: {device_info.get('serial_number', 'N/A')}")
            print(f"  Generated: {device_info.get('generated', 'N/A')}")
            print(f"  Temperature: {device_info.get('temperature', 'N/A')}")
    
    print(f"\n{'Metric':<25} {'Pre-Bake':<15} {'Post-Bake':<15} {'Change':<15} {'% Change':<10}")
    print("-" * 80)
    
    # Calculate and compare key metrics
    metrics = []
    
    # Voltage PGA 0 RMS error
    pre_v_rms = post_v_rms = None
    if 'voltage' in pre_bake_data:
        pre_v_pga0 = pre_bake_data['voltage'][pre_bake_data['voltage']['pga'] == 0]
        if not pre_v_pga0.empty:
            pre_v_rms = (pre_v_pga0['measurement_error_ppm']**2).mean()**0.5
    
    if 'voltage' in post_bake_data:
        post_v_pga0 = post_bake_data['voltage'][post_bake_data['voltage']['pga'] == 0]
        if not post_v_pga0.empty:
            post_v_rms = (post_v_pga0['measurement_error_ppm']**2).mean()**0.5
    
    if pre_v_rms is not None and post_v_rms is not None:
        change = post_v_rms - pre_v_rms
        pct_change = (change / pre_v_rms) * 100
        print(f"{'Voltage RMS (ppm)':<25} {pre_v_rms:<15.1f} {post_v_rms:<15.1f} {change:<15.1f} {pct_change:<10.1f}%")
    
    # Current PGA 2 RMS error
    pre_i_rms = post_i_rms = None
    if 'current' in pre_bake_data:
        pre_i_pga2 = pre_bake_data['current'][pre_bake_data['current']['pga'] == 2]
        if not pre_i_pga2.empty:
            pre_i_rms = (pre_i_pga2['measurement_error_ppm']**2).mean()**0.5
    
    if 'current' in post_bake_data:
        post_i_pga2 = post_bake_data['current'][post_bake_data['current']['pga'] == 2]
        if not post_i_pga2.empty:
            post_i_rms = (post_i_pga2['measurement_error_ppm']**2).mean()**0.5
    
    if pre_i_rms is not None and post_i_rms is not None:
        change = post_i_rms - pre_i_rms
        pct_change = (change / pre_i_rms) * 100
        print(f"{'Current RMS (ppm)':<25} {pre_i_rms:<15.1f} {post_i_rms:<15.1f} {change:<15.1f} {pct_change:<10.1f}%")
    
    # DAC RMS error
    pre_d_rms = post_d_rms = None
    if 'dac' in pre_bake_data:
        pre_d_rms = (pre_bake_data['dac']['measurement_error_ppm']**2).mean()**0.5
    
    if 'dac' in post_bake_data:
        post_d_rms = (post_bake_data['dac']['measurement_error_ppm']**2).mean()**0.5
    
    if pre_d_rms is not None and post_d_rms is not None:
        change = post_d_rms - pre_d_rms
        pct_change = (change / pre_d_rms) * 100
        print(f"{'DAC RMS (ppm)':<25} {pre_d_rms:<15.1f} {post_d_rms:<15.1f} {change:<15.1f} {pct_change:<10.1f}%")


def main():
    parser = argparse.ArgumentParser(description='Compare pre-bake and post-bake calibration data')
    parser.add_argument('pre_bake_file', help='Path to pre-bake HTML calibration report')
    parser.add_argument('post_bake_file', help='Path to post-bake HTML calibration report')
    parser.add_argument('--output', '-o', help='Output HTML file for the comparison plot')
    parser.add_argument('--no-show', action='store_true', help='Do not display the plot in browser')
    
    args = parser.parse_args()
    
    print("Loading calibration data...")
    
    # Load both datasets
    pre_bake_data = load_calibration_file(args.pre_bake_file, 'pre-bake')
    post_bake_data = load_calibration_file(args.post_bake_file, 'post-bake')
    
    if not pre_bake_data or not post_bake_data:
        print("Error: Could not load one or both calibration files")
        return
    
    # Print comparison summary
    print_comparison_summary(pre_bake_data, post_bake_data)
    
    # Create comparison plot
    print("\nCreating comparison plot...")
    fig = create_comparison_plot(pre_bake_data, post_bake_data)
    
    # Show what title was generated
    if 'device_info' in pre_bake_data and not pre_bake_data['device_info'].empty:
        device_info = pre_bake_data['device_info'].iloc[0]
        notes = device_info.get('notes', '')
        if notes:
            print(f"Plot title: {notes} ({device_info.get('serial_number', '')})")
        else:
            print("No 'Notes' field found in device info, using default title")
    
    # Save plot
    if args.output:
        output_path = Path(args.output)
    else:
        # Default output name
        pre_name = Path(args.pre_bake_file).stem
        output_path = Path(args.pre_bake_file).parent / f"{pre_name}_vs_post_bake_comparison.html"
    
    fig.write_html(output_path)
    print(f"Comparison plot saved to: {output_path}")
    
    # Show plot
    if not args.no_show:
        fig.show()


if __name__ == "__main__":
    main()