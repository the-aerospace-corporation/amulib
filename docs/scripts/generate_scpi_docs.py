#!/usr/bin/env python3
"""
SCPI Documentation Generator for AMU Library

This script reads SCPI command documentation from amu_commands.h and generates
comprehensive documentation in various formats. The documentation is now embedded
in the command enum definitions rather than the scpi.h macro definitions.

Author: Assistant
Date: 2024
"""

import re
import os
import sys
from dataclasses import dataclass
from typing import List, Dict, Optional
from pathlib import Path

@dataclass
class SCPICommand:
    """Represents a single SCPI command with documentation"""
    enum_name: str
    scpi_pattern: str
    description: str
    category: str
    usage: str = ""
    parameters: str = ""
    returns: str = ""
    
    @property
    def is_query(self) -> bool:
        """Check if this is a query command"""
        return '?' in self.scpi_pattern
    
    @property
    def command_name(self) -> str:
        """Get clean command name without optional brackets"""
        return self.scpi_pattern.replace('[', '').replace(']', '').replace('?', '')

class SCPIDocumentationGenerator:
    """Main class for parsing and generating SCPI documentation"""
    
    def __init__(self, source_dir: str):
        self.source_dir = Path(source_dir)
        self.commands: List[SCPICommand] = []
        
    def parse_scpi_commands(self) -> None:
        """Parse SCPI commands from amu_commands.h file where documentation is now stored"""
        commands_file = self.source_dir / "src" / "amulibc" / "amu_commands.h"
        
        if not commands_file.exists():
            print(f"Error: {commands_file} not found")
            return
            
        content = commands_file.read_text()
        
        # Parse documented enums with SCPI patterns
        self._parse_command_enums(content)
    
    def _parse_command_enums(self, content: str) -> None:
        """Parse SCPI command enums with embedded documentation"""
        
        # Pattern to match documented enum entries with SCPI patterns
        # Looking for /** ... @scpi PATTERN ... */ followed by enum definition
        pattern = r'/\*\*\s*(.*?)\*/\s*(\w+)\s*[,=]'
        
        matches = re.finditer(pattern, content, re.DOTALL)
        
        for match in matches:
            doc_comment = match.group(1)
            enum_name = match.group(2)
            
            # Extract SCPI pattern from documentation
            scpi_match = re.search(r'@scpi_cmd\s+(\S+)', doc_comment)
            if not scpi_match:
                continue
                
            scpi_pattern = scpi_match.group(1)
            
            # Extract other documentation fields
            description = self._extract_doc_field(doc_comment, r'@description\s+(.*?)(?=@|$)')
            if not description:
                description = self._extract_doc_field(doc_comment, r'@brief\s+(.*?)(?=@|$)')
            
            category = self._extract_doc_field(doc_comment, r'@category\s+(.*?)(?=@|$)')
            if not category:
                category = self._determine_category_from_scpi(scpi_pattern)
            
            usage = self._extract_doc_field(doc_comment, r'@usage\s+(.*?)(?=@|$)')
            parameters = self._extract_doc_field(doc_comment, r'@param\s+(.*?)(?=@|$)')
            returns = self._extract_doc_field(doc_comment, r'@returns\s+(.*?)(?=@|$)')
            
            if not description:
                # Try to extract from general description after @brief
                desc_match = re.search(r'@brief\s+(.*?)(?:\n|$)', doc_comment)
                if desc_match:
                    description = desc_match.group(1).strip()
                else:
                    # Fallback to first line
                    desc_match = re.search(r'\s*([^@\n]+)', doc_comment.strip())
                    if desc_match:
                        description = desc_match.group(1).strip()
            
            cmd = SCPICommand(
                enum_name=enum_name,
                scpi_pattern=scpi_pattern,
                description=description or "No description available",
                category=category or "Uncategorized",
                usage=usage or "",
                parameters=parameters or "",
                returns=returns or ""
            )
            
            self.commands.append(cmd)
    
    def _extract_doc_field(self, doc_text: str, pattern: str) -> str:
        """Extract a specific documentation field using regex"""
        match = re.search(pattern, doc_text, re.DOTALL | re.IGNORECASE)
        if match:
            return match.group(1).strip().replace('\n', ' ').replace('*', '').strip()
        return ""
    
    def _determine_category_from_scpi(self, scpi_pattern: str) -> str:
        """Determine category based on SCPI command pattern"""
        if scpi_pattern.startswith('*'):
            return "IEEE 488.2 Common Commands"
        elif scpi_pattern.startswith('STAT'):
            return "Status Commands"  
        elif scpi_pattern.startswith('SYST'):
            return "System Commands"
        elif scpi_pattern.startswith('DUT'):
            return "Device Under Test Commands"
        elif scpi_pattern.startswith('MEAS'):
            return "Measurement Commands"
        elif scpi_pattern.startswith('ADC'):
            return "ADC Commands"
        elif scpi_pattern.startswith('SWEEP'):
            return "Sweep Commands"
        elif scpi_pattern.startswith('HEAT'):
            return "Heater Commands"
        elif scpi_pattern.startswith('DAC'):
            return "DAC Commands"
        elif scpi_pattern.startswith('MEM'):
            return "Memory Commands"
        elif scpi_pattern.startswith('SUNS'):
            return "Sun Sensor Commands"
        else:
            return "Other Commands"
    
    def generate_markdown_documentation(self) -> str:
        """Generate markdown documentation for all SCPI commands"""
        doc = []
        doc.append("# SCPI Command Reference")
        doc.append("")
        doc.append("This document provides a comprehensive reference for all SCPI commands supported by the AMU library.")
        doc.append("Documentation is extracted from the command enum definitions in `amu_commands.h`.")
        doc.append("")
        
        # Group commands by category
        categories = self._group_commands_by_category()
        
        for category, cmds in categories.items():
            doc.append(f"## {category}")
            doc.append("")
            
            for cmd in cmds:
                doc.append(f"### `{cmd.scpi_pattern}`")
                doc.append("")
                doc.append(f"**Description:** {cmd.description}")
                doc.append("")
                doc.append(f"**Enum:** `{cmd.enum_name}`")
                doc.append("")
                doc.append(f"**Type:** {'Query' if cmd.is_query else 'Set/Get'}")
                doc.append("")
                
                if cmd.usage:
                    doc.append(f"**Usage:** {cmd.usage}")
                    doc.append("")
                
                if cmd.parameters:
                    doc.append(f"**Parameters:** {cmd.parameters}")
                    doc.append("")
                
                if cmd.returns:
                    doc.append(f"**Returns:** {cmd.returns}")
                    doc.append("")
                
                doc.append("---")
                doc.append("")
        
        return "\n".join(doc)
    
    def _group_commands_by_category(self) -> Dict[str, List[SCPICommand]]:
        """Group commands by their documented category"""
        categories = {}
        
        for cmd in self.commands:
            category = cmd.category
            
            if category not in categories:
                categories[category] = []
            categories[category].append(cmd)
        
        # Sort commands within each category
        for category in categories:
            categories[category].sort(key=lambda x: x.scpi_pattern)
        
        return categories
    
    def generate_doxygen_groups(self) -> str:
        """Generate Doxygen group definitions based on documented categories"""
        doc = []
        doc.append("/**")
        doc.append(" * @file scpi_groups.dox")
        doc.append(" * @brief SCPI Command Group Definitions")
        doc.append(" * @details This file defines Doxygen groups for SCPI commands")
        doc.append(" *          based on documentation in amu_commands.h")
        doc.append(" */")
        doc.append("")
        
        categories = self._group_commands_by_category()
        
        for category, cmds in categories.items():
            # Create a Doxygen group name
            group_name = category.lower().replace(" ", "_").replace(".", "").replace("/", "_")
            
            doc.append(f"/**")
            doc.append(f" * @defgroup scpi_{group_name} {category}")
            doc.append(f" * @brief {category} for AMU Library")
            doc.append(f" * @{{")
            doc.append(f" */")
            doc.append("")
            
            for cmd in cmds:
                doc.append(f"/**")
                doc.append(f" * @brief {cmd.scpi_pattern}: {cmd.description}")
                doc.append(f" * @details Enum: {cmd.enum_name}")
                if cmd.usage:
                    doc.append(f" * Usage: {cmd.usage}")
                doc.append(f" */")
                doc.append("")
            
            doc.append(f"/** @}} */")
            doc.append("")
        
        return "\n".join(doc)

def main():
    """Main entry point"""
    if len(sys.argv) != 2:
        print("Usage: python generate_scpi_docs.py <source_directory>")
        print("Example: python generate_scpi_docs.py /path/to/amulib")
        sys.exit(1)
    
    source_dir = sys.argv[1]
    
    if not os.path.exists(source_dir):
        print(f"Error: Source directory '{source_dir}' does not exist")
        sys.exit(1)
    
    generator = SCPIDocumentationGenerator(source_dir)
    
    print("Parsing SCPI commands from amu_commands.h...")
    generator.parse_scpi_commands()
    
    if not generator.commands:
        print("Warning: No documented SCPI commands found")
        print("Make sure amu_commands.h contains enum entries with @scpi documentation")
        return
    
    print(f"Found {len(generator.commands)} documented SCPI commands")
    
    # Generate markdown documentation
    print("Generating markdown documentation...")
    markdown_doc = generator.generate_markdown_documentation()
    
    # Write to file
    output_dir = Path(source_dir) / "docs"
    output_dir.mkdir(exist_ok=True)
    
    markdown_file = output_dir / "scpi_commands.md"
    markdown_file.write_text(markdown_doc)
    print(f"Markdown documentation written to: {markdown_file}")
    
    # Generate Doxygen groups
    print("Generating Doxygen groups...")
    doxygen_groups = generator.generate_doxygen_groups()
    
    groups_file = output_dir / "scpi_groups.dox"
    groups_file.write_text(doxygen_groups)
    print(f"Doxygen groups written to: {groups_file}")
    
    print("Documentation generation complete!")
    print(f"Categories found: {list(generator._group_commands_by_category().keys())}")

if __name__ == "__main__":
    main()