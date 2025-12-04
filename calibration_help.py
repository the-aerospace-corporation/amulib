#!/usr/bin/env python3
"""
Example usage and help for the calibration comparison scripts.
"""

def print_usage_examples():
    print("="*80)
    print("AMU CALIBRATION DATA ANALYSIS SCRIPTS - USAGE EXAMPLES")
    print("="*80)
    
    print("\n1. EXTRACT DATA FROM SINGLE CALIBRATION REPORT:")
    print("-" * 50)
    print("# Extract all data to CSV files")
    print("python extract_calibration_data.py report.html --csv")
    print()
    print("# Extract data and create interactive plots")
    print("python extract_calibration_data.py report.html --csv --plot")
    print()
    print("# Specify custom output directory")
    print("python extract_calibration_data.py report.html --csv --output-dir ./analysis/")
    
    print("\n2. ANALYZE EXTRACTED DATA:")
    print("-" * 30)
    print("# Run analysis on extracted CSV files")
    print("python analyze_calibration_data.py")
    print("# (Modify the base_path variable in the script to point to your data)")
    
    print("\n3. COMPARE PRE-BAKE vs POST-BAKE:")
    print("-" * 35)
    print("# Compare two calibration reports")
    print("python compare_calibration_data.py pre_bake_report.html post_bake_report.html")
    print()
    print("# Save comparison to specific file")
    print("python compare_calibration_data.py pre_bake.html post_bake.html --output comparison.html")
    print()
    print("# Run comparison without showing plot in browser")
    print("python compare_calibration_data.py pre_bake.html post_bake.html --no-show")
    
    print("\n4. WHAT THE COMPARISON SCRIPT SHOWS:")
    print("-" * 40)
    print("✓ Voltage Accuracy for PGA 0 (highest voltage range)")
    print("✓ Current Accuracy for PGA 2 (600mA range)")  
    print("✓ DAC Accuracy across all setpoints")
    print("✓ Side-by-side statistical comparison")
    print("✓ Temperature and timing information")
    print("✓ RMS error calculations and percent changes")
    
    print("\n5. OUTPUT FILES:")
    print("-" * 15)
    print("extract_calibration_data.py creates:")
    print("  • *_voltage_plot.csv       - Voltage measurement data")
    print("  • *_current_plot.csv       - Current measurement data") 
    print("  • *_dac_plot.csv          - DAC measurement data")
    print("  • *_voltage_coefficients.csv - Voltage calibration coefficients")
    print("  • *_current_coefficients.csv - Current calibration coefficients")
    print("  • *_dac_coefficients.csv  - DAC calibration coefficients")
    print("  • *_device_info.csv       - Device information")
    print()
    print("compare_calibration_data.py creates:")
    print("  • *_vs_post_bake_comparison.html - Interactive comparison plot")
    
    print("\n6. SCRIPT FEATURES:")
    print("-" * 20)
    print("✓ Automatic encoding detection (handles UTF-8, Latin-1, etc.)")
    print("✓ Robust JSON parsing from embedded Plotly data")
    print("✓ Interactive Plotly visualizations")
    print("✓ Statistical analysis and comparison")
    print("✓ Professional-quality plots ready for reports")
    print("✓ CSV export for further analysis in Excel/Python")
    
    print("\n7. TYPICAL WORKFLOW:")
    print("-" * 20)
    print("1. Run pre-bake calibration → generates pre_bake_report.html")
    print("2. Perform baking process")
    print("3. Run post-bake calibration → generates post_bake_report.html")
    print("4. python compare_calibration_data.py pre_bake_report.html post_bake_report.html")
    print("5. Review the generated comparison plot and statistics")
    
    print("\n" + "="*80)

if __name__ == "__main__":
    print_usage_examples()