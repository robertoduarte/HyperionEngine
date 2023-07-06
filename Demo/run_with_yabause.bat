@ECHO Off
call yaul\scripts\setenv.bat

where /q yabause.exe
IF ERRORLEVEL 1 (
    echo "Using yaul-win64 yabause installation!"
    SET YABAUSE=%YAUL_ROOT%/emulators/yabause/yabause.exe
) else (
    echo "Using system's yabause installation!"
    SET YABAUSE=yabause.exe
)

if not exist *.cue (
    echo "CUE/ISO missing, please build first."
    pause
) else (
    @REM Finding first cue file and running it on yabause
    FOR %%F IN (*.cue) DO (
        start "" %YABAUSE% -a -i %%F
        exit /b
    )
)
