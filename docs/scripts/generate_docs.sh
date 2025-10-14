#!/bin/bash
# SCPI Documentation Generator Wrapper Script
# This script ensures the conda environment is activated before running the Python script

echo "Activating conda environment..."
source C:/Users/cjm28241/AppData/Local/miniforge3/Scripts/activate base

echo "Generating SCPI documentation..."
cd /c/Users/cjm28241/source/repos/amulib
python docs/scripts/generate_scpi_docs.py .

echo "Done!"