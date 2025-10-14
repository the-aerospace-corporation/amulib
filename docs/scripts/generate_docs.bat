@echo off
REM SCPI Documentation Generator Wrapper Script
REM This script ensures the conda environment is activated before running the Python script

echo Activating conda environment...
call C:\Users\cjm28241\AppData\Local\miniforge3\Scripts\activate.bat base

echo Generating SCPI documentation...
python docs\scripts\generate_scpi_docs.py .

echo Done!
pause