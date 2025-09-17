#!/usr/bin/env python3
"""
SCPI Command Documentation Generator for AMU Library

This script parses the scpi.h file to extract SCPI command definitions
and generates comprehensive Doxygen documentation linking SCPI commands
to their command IDs and descriptions.

Usage: python generate_scpi_docs.py

Output: Updates scpi.h with Doxygen comments for each SCPI command
"""

import re
import sys
from pathlib import Path
from typing import Dict, List, Tuple, NamedTuple

class SCPICommand(NamedTuple):
    """Represents a parsed SCPI command"""
    scpi_string: str
    handler_function: str
    command_id: str
    description: str = ""
    group: str = ""
    parameters: str = ""
    returns: str = ""

def parse_scpi_commands(scpi_file_path: str) -> List[SCPICommand]:
    """Parse SCPI_COMMAND macros from scpi.h file"""
    commands = []
    
    with open(scpi_file_path, 'r') as f:
        content = f.read()
    
    # Pattern to match SCPI_COMMAND("pattern", handler, command_id)
    pattern = r'SCPI_COMMAND\(\s*"([^"]+)"\s*,\s*([^,]+)\s*,\s*([^)]+)\s*\)'
    
    matches = re.findall(pattern, content, re.MULTILINE)
    
    for scpi_string, handler, cmd_id in matches:
        cmd = SCPICommand(
            scpi_string=scpi_string.strip(),
            handler_function=handler.strip(),
            command_id=cmd_id.strip(),
            description=generate_description(scpi_string),
            group=determine_group(scpi_string),
            parameters=extract_parameters(scpi_string),
            returns=determine_return_type(scpi_string, handler)
        )
        commands.append(cmd)
    
    return commands

def generate_description(scpi_string: str) -> str:
    """Generate a description based on the SCPI command string"""
    descriptions = {
        # Standard SCPI commands
        "*CLS": "Clear status command - clears device status registers",
        "*ESE?": "Query event status enable register",
        "*ESR?": "Query event status register", 
        "*IDN?": "Query device identification string",
        "*OPC": "Operation complete command",
        "*OPC?": "Query operation complete status",
        "*RST": "Reset device to default state",
        "*SRE": "Set service request enable register",
        "*SRE?": "Query service request enable register",
        "*STB?": "Query status byte register",
        "*TST?": "Self-test query",
        "*WAI": "Wait for all operations to complete",
        
        # System commands
        "SYSTem:ERRor[:NEXT]?": "Query next error in error queue",
        "SYSTem:ERRor:COUNt?": "Query number of errors in error queue",
        "SYSTem:VERSion?": "Query SCPI version",
        "SYSTem:BOOTloader": "Enter bootloader mode for firmware update",
        "SYSTem:FIRMware?": "Query firmware version string",
        "SYSTem:HARDware?": "Query hardware revision number",
        "SYSTem:SERial?": "Query device serial number",
        "SYSTem:SLEEP": "Enter low-power sleep mode",
        "SYSTem:STATus?": "Query system status flags",
        "SYSTem:TEMPerature?": "Query internal temperature in Celsius",
        "SYSTem:TIMEstamp[?]": "Set or query system timestamp",
        "SYSTem:TIMEstamp:UTC[?]": "Set or query UTC timestamp",
        "SYSTem:TWI:ADDress[?]": "Set or query I2C device address",
        "SYSTem:TWI:NUMdevices?": "Query number of I2C devices detected",
        "SYSTem:TWI:MODE[?]": "Set or query I2C operating mode",
        "SYSTem:TWI:SCAN?": "Scan I2C bus and return device addresses",
        "SYSTem:TWI:STATus?": "Query I2C interface status",
        
        # LED commands  
        "SYSTem:LED:PAT": "Set LED pattern mode",
        "SYSTem:LED:COLOR[?]": "Set or query LED RGB color values",
        
        # Measurement commands
        "MEASure:ADC:ACTive[:RAW]?": "Measure all active ADC channels",
        "MEASure:INTERNALvoltages?": "Measure internal supply voltages",
        "MEASure:SUNSensor?": "Measure sun sensor angles",
        "MEASure:PRESSure?": "Measure pressure sensor data",
        
        # ADC commands
        "ADC:CALibrate[?]": "Perform or query ADC calibration",
        "ADC:CALibrate:TSENSor": "Calibrate temperature sensor at 25°C",
        "ADC:CALibrate:ALL:INTernal": "Calibrate all internal ADC channels",
        "ADC:SAVE:ALL:INTernal": "Save all internal calibrations to EEPROM",
        "ADC:CH:ACTIVE[?]": "Set or query active ADC channel mask",
        
        # Channel-specific commands (# = channel number)
        "ADC:CH#[?]": "Set or query ADC channel register value",
        "ADC:CH#:SETup[?]": "Set or query ADC channel setup configuration",
        "ADC:CH#:FILTer[?]": "Set or query ADC channel filter settings",
        "ADC:CH#:RATE[?]": "Set or query ADC channel sample rate",
        "ADC:CH#:PGA[?]": "Set or query programmable gain amplifier setting",
        "ADC:CH#:MAX?": "Query maximum input range for current PGA setting",
        "ADC:CH#:SAVE": "Save channel configuration to EEPROM",
        "ADC:CH#:OFFset[?]": "Set or query channel offset calibration coefficient",
        "ADC:CH#:GAIN[?]": "Set or query channel gain calibration coefficient",
        
        # Calibration commands
        "ADC:CH#:CALibrate:INTernal": "Perform internal calibration for channel",
        "ADC:CH#:CALibrate:ZERO": "Perform zero-scale calibration for channel",
        "ADC:CH#:CALibrate:FULL": "Perform full-scale calibration for channel", 
        "ADC:CH#:CALibrate:RESet": "Reset channel calibration to defaults",
        "ADC:CH#:CALibrate:SAVe": "Save channel calibration to EEPROM",
    }
    
    # Try exact match first
    if scpi_string in descriptions:
        return descriptions[scpi_string]
    
    # Try pattern matching for commands with variable parts
    for pattern, desc in descriptions.items():
        if pattern.replace('#', r'\d+').replace('[?]', r'(\?)?') in scpi_string:
            return desc
    
    # Generate generic description based on command structure
    if scpi_string.endswith('?'):
        return f"Query {scpi_string[:-1].lower()} value"
    else:
        return f"Set {scpi_string.lower()} value"

def determine_group(scpi_string: str) -> str:
    """Determine the Doxygen group for a command based on its prefix"""
    if scpi_string.startswith('*'):
        return "scpi_ieee488_commands"
    elif scpi_string.startswith('STATus'):
        return "scpi_status_commands"
    elif scpi_string.startswith('SYSTem'):
        return "scpi_system_commands"
    elif scpi_string.startswith('MEASure'):
        return "scpi_measurement_commands"
    elif scpi_string.startswith('DUT'):
        return "scpi_dut_commands"
    elif scpi_string.startswith('ADC'):
        return "scpi_adc_commands"
    elif scpi_string.startswith('SWEEP'):
        return "scpi_sweep_commands"
    elif scpi_string.startswith('DAC'):
        return "scpi_dac_commands"
    elif scpi_string.startswith('HEATer'):
        return "scpi_heater_commands"
    elif scpi_string.startswith('SUNSensor'):
        return "scpi_sunsensor_commands"
    elif scpi_string.startswith('MEMory'):
        return "scpi_memory_commands"
    else:
        return "scpi_commands"

def extract_parameters(scpi_string: str) -> str:
    """Extract parameter information from SCPI command"""
    if '[?]' in scpi_string:
        return "Optional query parameter"
    elif '?' in scpi_string:
        return "Query only - no parameters"
    elif '#' in scpi_string:
        return "Channel number (0-15)"
    else:
        return "Command parameters vary by function"

def determine_return_type(scpi_string: str, handler: str) -> str:
    """Determine return type based on command characteristics"""
    if not scpi_string.endswith('?'):
        return "Command acknowledgment"
    
    if 'float' in handler.lower():
        return "Floating-point value"
    elif 'uint8' in handler.lower():
        return "8-bit unsigned integer"
    elif 'uint16' in handler.lower():
        return "16-bit unsigned integer"
    elif 'uint32' in handler.lower():
        return "32-bit unsigned integer"
    elif 'str' in handler.lower() or 'query_str' in handler.lower():
        return "String value"
    else:
        return "Numeric value"

def generate_doxygen_comment(cmd: SCPICommand) -> str:
    """Generate Doxygen comment block for a SCPI command"""
    comment = f"""/**
 * @brief {cmd.description}
 * @ingroup {cmd.group}
 * 
 * **SCPI Command:** `{cmd.scpi_string}`
 * 
 * **Handler:** {cmd.handler_function}  
 * **Command ID:** {cmd.command_id}
 * 
 * @param {cmd.parameters}
 * @return {cmd.returns}
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("{cmd.scpi_string}");
 * @endcode
 */"""
    return comment

def update_scpi_file_with_docs(scpi_file_path: str, commands: List[SCPICommand]) -> None:
    """Update the scpi.h file with Doxygen documentation"""
    with open(scpi_file_path, 'r') as f:
        content = f.read()
    
    # For each command, add documentation before the SCPI_COMMAND line
    for cmd in commands:
        pattern = f'SCPI_COMMAND\\(\\s*"{re.escape(cmd.scpi_string)}"\\s*,\\s*{re.escape(cmd.handler_function)}\\s*,\\s*{re.escape(cmd.command_id)}\\s*\\)'
        
        # Check if documentation already exists
        doc_pattern = f'/\\*\\*[^*]*\\*/\\s*SCPI_COMMAND\\(\\s*"{re.escape(cmd.scpi_string)}"'
        if re.search(doc_pattern, content, re.DOTALL):
            continue  # Skip if already documented
        
        replacement = generate_doxygen_comment(cmd) + '\n        ' + f'SCPI_COMMAND("{cmd.scpi_string}", {cmd.handler_function}, {cmd.command_id})'
        
        content = re.sub(pattern, replacement, content)
    
    # Write updated content back to file
    with open(scpi_file_path, 'w') as f:
        f.write(content)

def main():
    """Main function to generate SCPI documentation"""
    script_dir = Path(__file__).parent
    scpi_file = script_dir.parent / "src" / "amulibc" / "scpi.h"
    
    if not scpi_file.exists():
        print(f"Error: Could not find scpi.h at {scpi_file}")
        return 1
    
    print("Parsing SCPI commands...")
    commands = parse_scpi_commands(str(scpi_file))
    print(f"Found {len(commands)} SCPI commands")
    
    print("Generating documentation...")
    update_scpi_file_with_docs(str(scpi_file), commands)
    
    print("Generating command summary...")
    generate_command_summary(commands, script_dir / "scpi_command_summary.md")
    
    print("Documentation generation complete!")
    return 0

def generate_command_summary(commands: List[SCPICommand], output_file: Path) -> None:
    """Generate a markdown summary of all commands"""
    with open(output_file, 'w') as f:
        f.write("# AMU SCPI Command Reference\n\n")
        f.write("This document provides a complete reference for all SCPI commands supported by the AMU.\n\n")
        
        # Group commands by category
        groups = {}
        for cmd in commands:
            if cmd.group not in groups:
                groups[cmd.group] = []
            groups[cmd.group].append(cmd)
        
        for group_name, group_commands in sorted(groups.items()):
            group_title = group_name.replace('scpi_', '').replace('_', ' ').title()
            f.write(f"## {group_title}\n\n")
            
            for cmd in sorted(group_commands, key=lambda x: x.scpi_string):
                f.write(f"### `{cmd.scpi_string}`\n\n")
                f.write(f"{cmd.description}\n\n")
                f.write(f"- **Handler:** {cmd.handler_function}\n")
                f.write(f"- **Command ID:** {cmd.command_id}\n")
                f.write(f"- **Parameters:** {cmd.parameters}\n")
                f.write(f"- **Returns:** {cmd.returns}\n\n")
                f.write("---\n\n")

if __name__ == "__main__":
    sys.exit(main())