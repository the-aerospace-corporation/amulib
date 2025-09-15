# AMU Library Documentation System

This directory contains the documentation generation system for the AMU (Aerospace Measurement Unit) library using Doxygen.

## Overview

The documentation system provides comprehensive coverage of:

- **SCPI Command Reference**: Complete documentation of all 100+ SCPI commands
- **C++ API Documentation**: Full library interface documentation  
- **Code Examples**: Working examples for common use cases
- **Hardware Reference**: AMU device specifications and interface details

## Files

- `Doxyfile` - Main Doxygen configuration file
- `mainpage.dox` - Main documentation page with overview
- `scpi_groups.dox` - SCPI command group definitions
- `scpi_command_reference.dox` - SCPI command reference page
- `generate_scpi_docs.py` - Python script to auto-generate SCPI documentation

## Generating Documentation

### Prerequisites

1. Install Doxygen: https://www.doxygen.nl/download.html
2. Optional: Install Graphviz for diagrams: https://graphviz.org/download/

### Build Documentation

```bash
# Navigate to docs directory
cd docs

# Generate documentation
doxygen Doxyfile

# Open generated documentation
# The documentation will be generated in docs/html/
# Open docs/html/index.html in your web browser
```

### Auto-Generate SCPI Documentation

To automatically generate SCPI command documentation from source code:

```bash
# Run the Python script to parse SCPI commands
python generate_scpi_docs.py

# This will:
# 1. Parse scpi.h for SCPI_COMMAND definitions
# 2. Add Doxygen comments to each command
# 3. Generate a markdown summary
```

## Documentation Structure

The documentation is organized as follows:

```
AMU Library Documentation
├── Introduction & Getting Started
├── SCPI Command Reference
│   ├── System Commands
│   ├── Measurement Commands  
│   ├── DUT Commands
│   ├── ADC Commands
│   ├── Sweep Commands
│   ├── DAC Commands
│   ├── Heater Commands
│   ├── Sun Sensor Commands
│   └── Memory Commands
├── C++ API Reference
├── Code Examples
└── Hardware Reference
```

## Command Documentation Format

Each SCPI command is documented with:

- **SCPI Syntax**: The exact command string
- **Description**: What the command does
- **Parameters**: Input parameters and ranges
- **Returns**: Expected response format
- **Example**: Usage example
- **Interface**: Available on I2C/USB
- **Command ID**: Internal command identifier

Example:

```c
/**
 * @brief Measure voltage on the dedicated voltage input channel
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:VOLTage[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_VOLTAGE
 * 
 * @param Optional :RAW suffix returns raw ADC value instead of calibrated voltage
 * @return Voltage measurement in volts (or raw ADC counts if :RAW specified)
 * 
 * @par Example:
 * @code
 * // Measure calibrated voltage
 * send_scpi_command("MEASure:ADC:VOLTage?");
 * // Returns: "12.345"
 * 
 * // Measure raw ADC value  
 * send_scpi_command("MEASure:ADC:VOLTage:RAW?");
 * // Returns: "8388608"
 * @endcode
 */
```

## Adding New Commands

When adding new SCPI commands:

1. Add the command to the appropriate enum in `amu_commands.h` with documentation
2. Add the SCPI_COMMAND entry in `scpi.h` 
3. Run `generate_scpi_docs.py` to auto-generate documentation
4. Rebuild documentation with `doxygen Doxyfile`

## Customization

To customize the documentation:

- Edit `Doxyfile` for Doxygen settings
- Modify `mainpage.dox` for the main page content
- Update `scpi_groups.dox` for command grouping
- Adjust CSS styles in the generated HTML directory

## Output

The generated documentation includes:

- **HTML**: Interactive web-based documentation
- **Search**: Full-text search capability
- **Cross-references**: Linked references between sections
- **Source code**: Syntax-highlighted source code browser
- **Call graphs**: Function relationship diagrams (if Graphviz installed)

## Maintenance

The documentation system is designed to:

- Auto-extract comments from source code
- Maintain consistency across all commands
- Generate user-friendly reference manuals
- Support both web and offline viewing
- Scale as new commands are added

For questions or issues with the documentation system, contact the library maintainer.