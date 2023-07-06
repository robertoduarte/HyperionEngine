@ECHO Off
call yaul\scripts\setenv.bat

where /q mednafen.exe
IF ERRORLEVEL 1 (
    echo "Using yaul-win64 mednafen installation!"
    SET MEDNAFEN=%YAUL_ROOT%/emulators/mednafen/mednafen.exe
) else (
    echo "Using system's mednafen installation!"
    SET MEDNAFEN=mednafen.exe
)

if not exist *.cue (
    echo "CUE/ISO missing, please build first."
    pause
) else (
    @REM Finding first cue file and running it on mednafen
    FOR %%F IN (*.cue) DO (
        start "" /MIN %MEDNAFEN% %%F
        exit /b
    )
)
