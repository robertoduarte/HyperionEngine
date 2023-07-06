@ECHO off

IF NOT EXIST yaul/ (
    powershell -Command "Invoke-Webrequest https://github.com/robertoduarte/yaul-win64/releases/download/v0.2.0-2023.06.19/yaul-0.2.0-win64-20230619.zip -Outfile yaul.zip"
    powershell -Command "Expand-Archive -Force yaul.zip"
    del yaul.zip
)

call yaul\scripts\setenv.bat
make
pause
