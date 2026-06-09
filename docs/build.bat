@echo off
REM Build the AMU docs: pull the theme submodule (first run) then run Doxygen.
REM Works from any directory.
cd /d "%~dp0"
git submodule update --init --recursive
doxygen Doxyfile
echo Docs built -^> %cd%\html\index.html
