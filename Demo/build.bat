@ECHO off

IF NOT EXIST yaul/ (
    powershell -Command "Invoke-Webrequest https://github.com/robertoduarte/yaul-win64/releases/download/v0.3.1-2024.01.25/yaul-0.3.1-win64-20240125.zip -Outfile yaul.zip"
    powershell -Command "Expand-Archive -Force yaul.zip"
    del yaul.zip
)

call yaul\scripts\setenv.bat
make clean
make
