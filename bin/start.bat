@echo off

setlocal

set para=x64\Release

if "%1" NEQ "" set para=%1

set Dir=%~dp0%para%\

@ping -n 1 127.0.0.1>nul
start "cdron_video_analysis" /D %Dir% %Dir%cdron_video_analysis.exe ..\..\server.cfg  ..\..\log
 

REM >face_api.log 2>&1 &

REM pause
