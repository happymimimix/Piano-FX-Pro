Setlocal enabledelayedexpansion
cd "%~dp0"
CHCP 65001 >nul
title Piano FX Studio v2.03
color 0f
echo off
for /l %%a in (3 1 25) do (
set /a "cols=30+%%a*2"
call mode con:cols=%%cols%% lines=%%a >nul
)
echo|set/p=[2J
goto Setup
:MAIN
ECHO.[H[?25l[40m
ECHO.                                                                                
ECHO.       [97m╔════════════════════════════════════════════════════════════════╗       
ECHO.       [97m║                   [93m┌────────────────────────┐                   [97m║       
ECHO.       [97m║ [91m»»»»»»»»»»»»»»»»» [93m│ [95mPiano FX Studio v2.03  [93m│ [91m««««««««««««««««« [97m║       
ECHO.       [97m║                   [93m│ [95mMade by: Happy_mimimix [93m│                   [97m║       
ECHO.       [97m║                   [93m└────────────────────────┘                   [97m║       
ECHO.       [97m║                                                                ║       
ECHO.       [97m║                                                                ║       
ECHO.       [97m║                                                                ║       
ECHO.       [97m║                                                                ║       
ECHO.       [97m║                                                                ║       
ECHO.       [97m║                                                                ║       
ECHO.       [97m║                                                                ║       
ECHO.       [97m║                                                                ║       
ECHO.       [97m║                                                                ║       
ECHO.       [97m║                                                                ║       
ECHO.       [97m║                                                                ║       
ECHO.       [97m║                                                                ║       
ECHO.       [97m║                                                                ║       
ECHO.       [97m║                                                                ║       
ECHO.       [97m║                                                                ║       
ECHO.       [97m╚════════════════════════════════════════════════════════════════╝       
ECHO.                                                                                
exit /b
:Setup
call :MAIN
ECHO|set/p=[21;10H[97m┌───────────┐ ┌─────────────┐ ┌───────────┐
ECHO|set/p=[22;10H[97m│ [91m(1) Setup[97m │ │ [90m(2) Animate[97m │ │ [90m(3) Color[97m │
ECHO|set/p=[23;10H[97m╧═══════════╧═╧═════════════╧═╧═══════════╧
ECHO|set/p=[8;12H[40m[97m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[9;12H[107m[30m (A) Install Black MIDI templates for FL Studio 12.3      [100m [40m
ECHO|set/p=[10;12H[40m[97m▀[100m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ [40m
ECHO|set/p=[11;12H[40m[97m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[12;12H[107m[30m (B) Download ffmpeg.exe from official website            [100m [40m
ECHO|set/p=[13;12H[40m[97m▀[100m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ [40m
ECHO|set/p=[14;12H[40m[97m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[15;12H[107m[30m (C) Download Cheat Engine from official website          [100m [40m
ECHO|set/p=[16;12H[40m[97m▀[100m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ [40m
ECHO|set/p=[17;12H[40m[97m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[18;12H[107m[30m (D) Run Piano FX Pro v2.03                               [100m [40m
ECHO|set/p=[19;12H[40m[97m▀[100m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ [40m
CHOICE /C 23abcd /N >nul
if "%errorlevel%" equ "1" (goto Animate)
if "%errorlevel%" equ "2" (goto Color)
if "%errorlevel%" equ "3" (call :SA & goto Setup)
if "%errorlevel%" equ "4" (call :SB & goto Setup)
if "%errorlevel%" equ "5" (call :SC & goto Setup)
if "%errorlevel%" equ "6" (call :SD & goto Setup)
:Animate
call :MAIN
ECHO|set/p=[21;10H[97m┌───────────┐ ┌─────────────┐ ┌───────────┐
ECHO|set/p=[22;10H[97m│ [90m(1) Setup[97m │ │ [91m(2) Animate[97m │ │ [90m(3) Color[97m │
ECHO|set/p=[23;10H[97m╧═══════════╧═╧═════════════╧═╧═══════════╧
ECHO|set/p=[8;12H[40m[97m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[9;12H[107m[30m (A) Copy lua script template to clipboard                [100m [40m
ECHO|set/p=[10;12H[40m[97m▀[100m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ [40m
ECHO|set/p=[11;12H[40m[97m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[12;12H[107m[30m (B) Generate animation code with fields                  [100m [40m
ECHO|set/p=[13;12H[40m[97m▀[100m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ [40m
ECHO|set/p=[14;12H[40m[97m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[15;12H[107m[30m (C) Advanced visualize pitch bend tricks tutorial        [100m [40m
ECHO|set/p=[16;12H[40m[97m▀[100m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ [40m
ECHO|set/p=[17;12H[40m[97m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[18;12H[107m[30m (D) Cheat engine lua scripting tutorial                  [100m [40m
ECHO|set/p=[19;12H[40m[97m▀[100m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ [40m
CHOICE /C 13abcd /N >nul
if "%errorlevel%" equ "1" (goto Setup)
if "%errorlevel%" equ "2" (goto Color)
if "%errorlevel%" equ "3" (call :AA & goto Animate)
if "%errorlevel%" equ "4" (call :AB & goto Animate)
if "%errorlevel%" equ "5" (call :AC & goto Animate)
if "%errorlevel%" equ "6" (call :AD & goto Animate)
:Color
call :MAIN
ECHO|set/p=[21;10H[97m┌───────────┐ ┌─────────────┐ ┌───────────┐
ECHO|set/p=[22;10H[97m│ [90m(1) Setup[97m │ │ [90m(2) Animate[97m │ │ [91m(3) Color[97m │
ECHO|set/p=[23;10H[97m╧═══════════╧═╧═════════════╧═╧═══════════╧
ECHO|set/p=[8;12H[40m[97m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[9;12H[107m[30m (A) Generate color events from lyric events              [100m [40m
ECHO|set/p=[8;12H[40m[97m▀[100m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ [40m
ECHO|set/p=[8;12H[40m[97m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[9;12H[107m[30m (B) Generate color events from PFA config                [100m [40m
ECHO|set/p=[8;12H[40m[97m▀[100m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ [40m
ECHO|set/p=[8;12H[40m[97m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[9;12H[107m[30m (C) Restore editable lyric events from color events      [100m [40m
ECHO|set/p=[8;12H[40m[97m▀[100m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ [40m
ECHO|set/p=[8;12H[40m[97m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[9;12H[107m[30m (D) Generate gradient color                              [100m [40m
ECHO|set/p=[8;12H[40m[97m▀[100m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ [40m
CHOICE /C 12abcd /N >nul
if "%errorlevel%" equ "1" (goto Setup)
if "%errorlevel%" equ "2" (goto Animate)
if "%errorlevel%" equ "3" (call :CA & goto Color)
if "%errorlevel%" equ "4" (call :CB & goto Color)
if "%errorlevel%" equ "5" (call :CA & goto Color)
if "%errorlevel%" equ "6" (call :CB & goto Color)
:SA
ECHO|set/p=[8;12H[40m[93m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[9;12H[103m[30m (A) Install Black MIDI templates for FL Studio 12.3      [40m 
ECHO|set/p=[10;12H[40m[93m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ 
timeout 1 /nobreak >nul
ECHO|set/p=[10;4H[104m[97m╔════════════════════════════════════════════════════════════════════════╗
ECHO|set/p=[11;4H[104m[97m║ Fl Studio installation DIR:                                            ║
ECHO|set/p=[12;4H[104m[97m║                                                                        ║
ECHO|set/p=[13;4H[104m[97m║                                                                        ║
ECHO|set/p=[14;4H[104m[97m║                                                                        ║
ECHO|set/p=[15;4H[104m[97m╚════════════════════════════════════════════════════════════════════════╝
set /P flpath=[13;7H[?25h
POWERSHELL -Command "Expand-Archive -Path "^""%~dp0FlTemplates.zip"^"" -DestinationPath "^""%flpath%\Data\Projects\Templates"^"" -Force"
exit /b
:SB
ECHO|set/p=[11;12H[40m[93m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[12;12H[103m[30m (B) Download ffmpeg.exe from official website            [40m 
ECHO|set/p=[13;12H[40m[93m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ 
timeout 1 /nobreak >nul
explorer http://github.com/BtbN/FFmpeg-Builds/releases
exit /b
:SC
ECHO|set/p=[14;12H[40m[93m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[15;12H[103m[30m (C) Download Cheat Engine from official website          [40m 
ECHO|set/p=[16;12H[40m[93m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ 
timeout 1 /nobreak >nul
explorer http://www.cheatengine.org/
exit /b
:SD
ECHO|set/p=[17;12H[40m[93m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[18;12H[103m[30m (D) Run Piano FX Pro v2.03                               [40m 
ECHO|set/p=[19;12H[40m[93m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ 
timeout 1 /nobreak >nul
start "" "%~dp0Piano-FX-Pro.exe"
exit /b
:AA
ECHO|set/p=[8;12H[40m[93m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[9;12H[103m[30m (A) Copy lua script template to clipboard                [40m 
ECHO|set/p=[10;12H[40m[93m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ 
ECHO.-- Definitions: (DO NOT CHANGE^^^!) >"%~dp0x"
ECHO.openProcess(^"Piano-FX-Pro.exe^") >>"%~dp0x"
ECHO.Microseconds=^"Piano-FX-Pro.exe+941650^" >>"%~dp0x"
ECHO.Ticks=^"Piano-FX-Pro.exe+94165C^" >>"%~dp0x"
ECHO.Resolution=^"Piano-FX-Pro.exe+93D318^" >>"%~dp0x"
ECHO.Volume=^"Piano-FX-Pro.exe+941508^" >>"%~dp0x"
ECHO.Mute=^"Piano-FX-Pro.exe+9414F6^" >>"%~dp0x"
ECHO.PlaybackSpeed=^"Piano-FX-Pro.exe+9414F8^" >>"%~dp0x"
ECHO.NoteSpeed=^"Piano-FX-Pro.exe+941500^" >>"%~dp0x"
ECHO.OffsetX=^"Piano-FX-Pro.exe+941520^" >>"%~dp0x"
ECHO.OffsetY=^"Piano-FX-Pro.exe+941524^" >>"%~dp0x"
ECHO.Zoom=^"Piano-FX-Pro.exe+941528^" >>"%~dp0x"
ECHO.StartKey=^"Piano-FX-Pro.exe+941424^" >>"%~dp0x"
ECHO.EndKey=^"Piano-FX-Pro.exe+941428^" >>"%~dp0x"
ECHO.KeyMode=^"Piano-FX-Pro.exe+941420^" >>"%~dp0x"
ECHO.Width=^"Piano-FX-Pro.exe+93D310^" >>"%~dp0x"
ECHO.Height=^"Piano-FX-Pro.exe+93D314^" >>"%~dp0x"
ECHO.Paused=^"Piano-FX-Pro.exe+9414F5^" >>"%~dp0x"
ECHO.Keyboard=^"Piano-FX-Pro.exe+941519^" >>"%~dp0x"
ECHO.VisualizePitchBends=^"Piano-FX-Pro.exe+941578^" >>"%~dp0x"
ECHO.PhigrosMode=^"Piano-FX-Pro.exe+941550^" >>"%~dp0x"
ECHO.ShowMarkers=^"Piano-FX-Pro.exe+941549^" >>"%~dp0x"
ECHO.TickBased=^"Piano-FX-Pro.exe+941548^" >>"%~dp0x"
ECHO.DisableUI=^"Piano-FX-Pro.exe+9415A2^" >>"%~dp0x"
ECHO.LimitFPS=^"Piano-FX-Pro.exe+9414C9^" >>"%~dp0x"
ECHO.Caption=^"Piano-FX-Pro.exe+93D320^" >>"%~dp0x"
ECHO.MS=1e+3 >>"%~dp0x"
ECHO.S=1e+6 >>"%~dp0x"
ECHO.function GetMicroseconds() >>"%~dp0x"
ECHO.return readQword(Microseconds) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetTicks() >>"%~dp0x"
ECHO.return readInteger(Ticks,true) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetPPQ() >>"%~dp0x"
ECHO.return readInteger(Resolution) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetVolume() >>"%~dp0x"
ECHO.return readDouble(Volume) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetMute() >>"%~dp0x"
ECHO.return readByte(Mute) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetPlaybackSpeed() >>"%~dp0x"
ECHO.return readDouble(PlaybackSpeed) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetNoteSpeed() >>"%~dp0x"
ECHO.return readDouble(NoteSpeed) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetStartKey() >>"%~dp0x"
ECHO.return readInteger(StartKey) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetEndKey() >>"%~dp0x"
ECHO.return readInteger(EndKey) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetKeyMode() >>"%~dp0x"
ECHO.return readInteger(KeyMod) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetOffsetX() >>"%~dp0x"
ECHO.return readFloat(OffsetX) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetOffsetY() >>"%~dp0x"
ECHO.return readFloat(OffsetY) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetZoom() >>"%~dp0x"
ECHO.return readFloat(Zoom) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetWidth() >>"%~dp0x"
ECHO.return readInteger(Width) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetHeight() >>"%~dp0x"
ECHO.return readInteger(Height) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetPaused() >>"%~dp0x"
ECHO.return readByte(Paused) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetKeyboard() >>"%~dp0x"
ECHO.return readByte(Keyboard) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetVisualizePitchBends() >>"%~dp0x"
ECHO.return readByte(VisualizePitchBends) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetPhigrosMode() >>"%~dp0x"
ECHO.return readByte(PhigrosMode) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetShowMarkers() >>"%~dp0x"
ECHO.return readByte(ShowMarkers) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetTickBased() >>"%~dp0x"
ECHO.return readByte(TickBased) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetDisableUI() >>"%~dp0x"
ECHO.return readByte(DisableUI) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetLimitFPS() >>"%~dp0x"
ECHO.return readByte(LimitFPS) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function GetCaption() >>"%~dp0x"
ECHO.return readString(Caption) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetMicroseconds(VAL) >>"%~dp0x"
ECHO.writeQword(Microseconds,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetVolume(VAL) >>"%~dp0x"
ECHO.writeDouble(Volume,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetMute(VAL) >>"%~dp0x"
ECHO.writeByte(Mute,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetPlaybackSpeed(VAL) >>"%~dp0x"
ECHO.writeDouble(PlaybackSpeed,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetNoteSpeed(VAL) >>"%~dp0x"
ECHO.writeDouble(NoteSpeed,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetStartKey(VAL) >>"%~dp0x"
ECHO.writeInteger(StartKey,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetEndKey(VAL) >>"%~dp0x"
ECHO.writeInteger(EndKey,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetKeyMode(VAL) >>"%~dp0x"
ECHO.writeInteger(KeyMode,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetOffsetX(VAL) >>"%~dp0x"
ECHO.writeFloat(OffsetX,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetOffsetY(VAL) >>"%~dp0x"
ECHO.writeFloat(OffsetY,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetZoom(VAL) >>"%~dp0x"
ECHO.writeFloat(Zoom,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetPaused(VAL) >>"%~dp0x"
ECHO.writeByte(Paused,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetKeyboard(VAL) >>"%~dp0x"
ECHO.writeByte(Keyboard,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetVisualizePitchBends(VAL) >>"%~dp0x"
ECHO.writeByte(VisualizePitchBends,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetPhigrosMode(VAL) >>"%~dp0x"
ECHO.writeByte(PhigrosMode,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetShowMarkers(VAL) >>"%~dp0x"
ECHO.writeByte(ShowMarkers,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetTickBased(VAL) >>"%~dp0x"
ECHO.writeByte(TickBased,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetDisableUI(VAL) >>"%~dp0x"
ECHO.writeByte(DisableUI,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetLimitFPS(VAL) >>"%~dp0x"
ECHO.writeByte(LimitFPS,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SetCaption(VAL) >>"%~dp0x"
ECHO.writeByte(getAddress(Caption)+#VAL,0);writeString(Caption,VAL) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseLinear(x) >>"%~dp0x"
ECHO.return x >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInSine(x) >>"%~dp0x"
ECHO.return 1-math.cos((x*math.pi)/2) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseOutSine(x) >>"%~dp0x"
ECHO.return math.sin((x*math.pi)/2) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInOutSine(x) >>"%~dp0x"
ECHO.return -(math.cos(math.pi*x)-1)/2 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInQuad(x) >>"%~dp0x"
ECHO.return x*x >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseOutQuad(x) >>"%~dp0x"
ECHO.return 1-(1-x)*(1-x) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInOutQuad(x) >>"%~dp0x"
ECHO.return x^<0.5 and 2*x*x or 1-math.pow(-2*x+2,2)/2 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInCubic(x) >>"%~dp0x"
ECHO.return x*x*x >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseOutCubic(x) >>"%~dp0x"
ECHO.return 1-math.pow(1-x,3) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInOutCubic(x) >>"%~dp0x"
ECHO.return x^<0.5 and 4*x*x*x or 1-math.pow(-2*x+2,3)/2 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInQuart(x) >>"%~dp0x"
ECHO.return x*x*x*x >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseOutQuart(x) >>"%~dp0x"
ECHO.return 1-math.pow(1-x,4) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInOutQuart(x) >>"%~dp0x"
ECHO.return x^<0.5 and 8*x*x*x*x or 1-math.pow(-2*x+2,4)/2 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInQuint(x) >>"%~dp0x"
ECHO.return x*x*x*x*x >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseOutQuint(x) >>"%~dp0x"
ECHO.return 1-math.pow(1-x,5) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInOutQuint(x) >>"%~dp0x"
ECHO.return x^<0.5 and 16*x*x*x*x*x or 1-math.pow(-2*x+2,5)/2 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInExpo(x) >>"%~dp0x"
ECHO.return x == 0 and 0 or math.pow(2,10*x-10) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseOutExpo(x) >>"%~dp0x"
ECHO.return x == 1 and 1 or 1-math.pow(2,-10*x) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInOutExpo(x) >>"%~dp0x"
ECHO.return x == 0 and 0 or x == 1 and 1 or x^<0.5 and math.pow(2,20*x-10)/2 or (2-math.pow(2,-20*x+10))/2 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInCirc(x) >>"%~dp0x"
ECHO.return 1-math.sqrt(1-math.pow(x,2)) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseOutCirc(x) >>"%~dp0x"
ECHO.return math.sqrt(1-math.pow(x-1,2)) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInOutCirc(x) >>"%~dp0x"
ECHO.return x^<0.5 and (1-math.sqrt(1-math.pow(2*x,2)))/2 or (math.sqrt(1-math.pow(-2*x+2,2))+1)/2 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInBack(x) >>"%~dp0x"
ECHO.local c1=1.70158 >>"%~dp0x"
ECHO.local c3=c1+1 >>"%~dp0x"
ECHO.return c3*x*x*x-c1*x*x >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseOutBack(x) >>"%~dp0x"
ECHO.local c1=1.70158 >>"%~dp0x"
ECHO.local c3=c1+1 >>"%~dp0x"
ECHO.return 1+c3*math.pow(x-1,3)+c1*math.pow(x-1,2) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInOutBack(x) >>"%~dp0x"
ECHO.local c1=1.70158 >>"%~dp0x"
ECHO.local c2=c1*1.525 >>"%~dp0x"
ECHO.return x^<0.5 and (math.pow(2*x,2)*((c2+1)*2*x-c2))/2 or (math.pow(2*x-2,2)*((c2+1)*(x*2-2)+c2)+2)/2 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInElastic(x) >>"%~dp0x"
ECHO.local c4=(2*math.pi)/3 >>"%~dp0x"
ECHO.return x == 0 and 0 or x == 1 and 1 or -math.pow(2,10*x-10)*math.sin((x*10-10.75)*c4) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseOutElastic(x) >>"%~dp0x"
ECHO.local c4=(2*math.pi)/3 >>"%~dp0x"
ECHO.return x == 0 and 0 or x == 1 and 1 or math.pow(2,-10*x)*math.sin((x*10-0.75)*c4)+1 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInOutElastic(x) >>"%~dp0x"
ECHO.local c5=(2*math.pi)/4.5 >>"%~dp0x"
ECHO.return x == 0 and 0 or x == 1 and 1 or x^<0.5 and -(math.pow(2,20*x-10)*math.sin((20*x-11.125)*c5))/2 or (math.pow(2,-20*x+10)*math.sin((20*x-11.125)*c5))/2+1 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInBounce(x) >>"%~dp0x"
ECHO.return 1-EaseOutBounce(1-x) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseOutBounce(x) >>"%~dp0x"
ECHO.local n1=7.5625 >>"%~dp0x"
ECHO.local d1=2.75 >>"%~dp0x"
ECHO.if x^<1/d1 then >>"%~dp0x"
ECHO.return n1*x*x >>"%~dp0x"
ECHO.elseif x^<2/d1 then >>"%~dp0x"
ECHO.return n1*(x-1.5/d1)*(x-1.5/d1)+0.75 >>"%~dp0x"
ECHO.elseif x^<2.5/d1 then >>"%~dp0x"
ECHO.return n1*(x-2.25/d1)*(x-2.25/d1)+0.9375 >>"%~dp0x"
ECHO.else >>"%~dp0x"
ECHO.return n1*(x-2.625/d1)*(x-2.625/d1)+0.984375 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function EaseInOutBounce(x) >>"%~dp0x"
ECHO.return x^<0.5 and (1-EaseOutBounce(1-2*x))/2 or (1+EaseOutBounce(2*x-1))/2 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SmoothAnimationByTime(StartValue,EndValue,StartTime,EndTime,EasingFunction,SetValueFunction) >>"%~dp0x"
ECHO.return coroutine.create(function() >>"%~dp0x"
ECHO.while (GetMicroseconds()^<StartTime and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.while (GetMicroseconds() ^<= EndTime and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.SetValueFunction(StartValue+EasingFunction((GetMicroseconds()-StartTime)/(EndTime-StartTime))*(EndValue-StartValue)) >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.SetValueFunction(EndValue) >>"%~dp0x"
ECHO.end) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function TargetAnimationByTime(GetValueFunction,TargetValue,StartTime,EndTime,EasingFunction,SetValueFunction) >>"%~dp0x"
ECHO.return coroutine.create(function() >>"%~dp0x"
ECHO.while (GetMicroseconds()^<StartTime and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.local StartValue=GetValueFunction() >>"%~dp0x"
ECHO.while (GetMicroseconds() ^<= EndTime and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.SetValueFunction(StartValue+EasingFunction((GetMicroseconds()-StartTime)/(EndTime-StartTime))*(TargetValue-StartValue)) >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.SetValueFunction(TargetValue) >>"%~dp0x"
ECHO.end) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SlicedSmoothAnimationByTime(StartValue,EndValue,StartTime,EndTime,Spacing,EasingFunction,SetValueFunction) >>"%~dp0x"
ECHO.return coroutine.create(function() >>"%~dp0x"
ECHO.while (GetMicroseconds()^<StartTime and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.local SliceIndex=1 >>"%~dp0x"
ECHO.while (GetMicroseconds() ^<= EndTime and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.SetValueFunction(StartValue+EasingFunction((GetMicroseconds()-StartTime)/(EndTime-StartTime))*(EndValue-StartValue)) >>"%~dp0x"
ECHO.while (GetMicroseconds()^<StartTime+Spacing*SliceIndex and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.SliceIndex=SliceIndex+1 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.SetValueFunction(EndValue) >>"%~dp0x"
ECHO.end) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SlicedTargetAnimationByTime(GetValueFunction,TargetValue,StartTime,EndTime,Spacing,EasingFunction,SetValueFunction) >>"%~dp0x"
ECHO.return coroutine.create(function() >>"%~dp0x"
ECHO.while (GetMicroseconds()^<StartTime and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.local StartValue=GetValueFunction() >>"%~dp0x"
ECHO.local SliceIndex=1 >>"%~dp0x"
ECHO.while (GetMicroseconds() ^<= EndTime and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.SetValueFunction(StartValue+EasingFunction((GetMicroseconds()-StartTime)/(EndTime-StartTime))*(TargetValue-StartValue)) >>"%~dp0x"
ECHO.while (GetMicroseconds()^<StartTime+Spacing*SliceIndex and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.SliceIndex=SliceIndex+1 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.SetValueFunction(TargetValue) >>"%~dp0x"
ECHO.end) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function InstantaneousAnimationByTime(Value,Time,SetValueFunction) >>"%~dp0x"
ECHO.return coroutine.create(function() >>"%~dp0x"
ECHO.while (GetMicroseconds()^<Time and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.SetValueFunction(Value) >>"%~dp0x"
ECHO.end) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SmoothAnimationByTick(StartValue,EndValue,StartTime,EndTime,EasingFunction,SetValueFunction) >>"%~dp0x"
ECHO.return coroutine.create(function() >>"%~dp0x"
ECHO.while (GetTicks()^<StartTime and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.while (GetTicks() ^<= EndTime and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.SetValueFunction(StartValue+EasingFunction((GetTicks()-StartTime)/(EndTime-StartTime))*(EndValue-StartValue)) >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.SetValueFunction(EndValue) >>"%~dp0x"
ECHO.end) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function TargetAnimationByTick(GetValueFunction,TargetValue,StartTime,EndTime,EasingFunction,SetValueFunction) >>"%~dp0x"
ECHO.return coroutine.create(function() >>"%~dp0x"
ECHO.while (GetTicks()^<StartTime and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.local StartValue=GetValueFunction() >>"%~dp0x"
ECHO.while (GetTicks() ^<= EndTime and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.SetValueFunction(StartValue+EasingFunction((GetTicks()-StartTime)/(EndTime-StartTime))*(TargetValue-StartValue)) >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.SetValueFunction(TargetValue) >>"%~dp0x"
ECHO.end) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SlicedSmoothAnimationByTick(StartValue,EndValue,StartTime,EndTime,Spacing,EasingFunction,SetValueFunction) >>"%~dp0x"
ECHO.return coroutine.create(function() >>"%~dp0x"
ECHO.while (GetTicks()^<StartTime and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.local SliceIndex=1 >>"%~dp0x"
ECHO.while (GetTicks() ^<= EndTime and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.SetValueFunction(StartValue+EasingFunction((GetTicks()-StartTime)/(EndTime-StartTime))*(EndValue-StartValue)) >>"%~dp0x"
ECHO.while (GetTicks()^<StartTime+Spacing*SliceIndex and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.SliceIndex=SliceIndex+1 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.SetValueFunction(EndValue) >>"%~dp0x"
ECHO.end) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function SlicedTargetAnimationByTick(GetValueFunction,TargetValue,StartTime,EndTime,Spacing,EasingFunction,SetValueFunction) >>"%~dp0x"
ECHO.return coroutine.create(function() >>"%~dp0x"
ECHO.while (GetTicks()^<StartTime and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.local StartValue=GetValueFunction() >>"%~dp0x"
ECHO.local SliceIndex=1 >>"%~dp0x"
ECHO.while (GetTicks() ^<= EndTime and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.SetValueFunction(StartValue+EasingFunction((GetTicks()-StartTime)/(EndTime-StartTime))*(TargetValue-StartValue)) >>"%~dp0x"
ECHO.while (GetTicks()^<StartTime+Spacing*SliceIndex and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.SliceIndex=SliceIndex+1 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.SetValueFunction(TargetValue) >>"%~dp0x"
ECHO.end) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function InstantaneousAnimationByTick(Value,Time,SetValueFunction) >>"%~dp0x"
ECHO.return coroutine.create(function() >>"%~dp0x"
ECHO.while (GetTicks()^<Time and GetTicks()^<TotalTicks) do >>"%~dp0x"
ECHO.coroutine.yield() >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.SetValueFunction(Value) >>"%~dp0x"
ECHO.end) >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function CenterX(Zoom) >>"%~dp0x"
ECHO.return (GetWidth()*(1-Zoom))/2 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function CenterY(Zoom) >>"%~dp0x"
ECHO.return -(GetHeight()*(1-Zoom))/2 >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.function RunThreads(Threads) >>"%~dp0x"
ECHO.local Working=true >>"%~dp0x"
ECHO.while Working do >>"%~dp0x"
ECHO.Working=false >>"%~dp0x"
ECHO.for _,ThisThread in ipairs(Threads) do >>"%~dp0x"
ECHO.if coroutine.status(ThisThread) ~= ^"dead^" then >>"%~dp0x"
ECHO.coroutine.resume(ThisThread) >>"%~dp0x"
ECHO.Working=true >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.end >>"%~dp0x"
ECHO.-- Initialization: >>"%~dp0x"
ECHO.TotalTicks=GetPPQ()*4*10 >>"%~dp0x"
ECHO.SetMicroseconds(-3*S) >>"%~dp0x"
ECHO.SetVolume(1.00) >>"%~dp0x"
ECHO.SetMute(0) >>"%~dp0x"
ECHO.SetPlaybackSpeed(1.00) >>"%~dp0x"
ECHO.SetNoteSpeed(0.25) >>"%~dp0x"
ECHO.SetStartKey(0) >>"%~dp0x"
ECHO.SetEndKey(127) >>"%~dp0x"
ECHO.SetKeyMode(0) >>"%~dp0x"
ECHO.SetOffsetX(0.00) >>"%~dp0x"
ECHO.SetOffsetY(0.00) >>"%~dp0x"
ECHO.SetZoom(0.00) >>"%~dp0x"
ECHO.SetPaused(0) >>"%~dp0x"
ECHO.SetKeyboard(1) >>"%~dp0x"
ECHO.SetVisualizePitchBends(1) >>"%~dp0x"
ECHO.SetPhigrosMode(1) >>"%~dp0x"
ECHO.SetShowMarkers(1) >>"%~dp0x"
ECHO.SetTickBased(1) >>"%~dp0x"
ECHO.SetDisableUI(0) >>"%~dp0x"
ECHO.SetLimitFPS(1) >>"%~dp0x"
ECHO.SetCaption(^"Welcome to Piano-FX Pro^") >>"%~dp0x"
ECHO.-- Main Program: >>"%~dp0x"
ECHO.ThreadList={} >>"%~dp0x"
ECHO.table.insert(ThreadList,SmoothAnimationByTime(0,1,-S,0,EaseOutBack,SetZoom)) >>"%~dp0x"
ECHO.table.insert(ThreadList,SmoothAnimationByTime(CenterX(0),CenterX(1),-S,0,EaseOutBack,SetOffsetX)) >>"%~dp0x"
ECHO.table.insert(ThreadList,SmoothAnimationByTime(CenterY(0),CenterY(1),-S,0,EaseOutBack,SetOffsetY)) >>"%~dp0x"
ECHO.table.insert(ThreadList,InstantaneousAnimationByTime(^"^",0,SetCaption)) >>"%~dp0x"
ECHO.RunThreads(ThreadList) >>"%~dp0x"
ECHO.--[[ >>"%~dp0x"
ECHO.Notice: >>"%~dp0x"
ECHO.Cheat Engine's UI may freeze and stop responding while the script is running. >>"%~dp0x"
ECHO.This is a normal phenomenon, please do not kill this process^^^! >>"%~dp0x"
ECHO.]] >>"%~dp0x"
type "%~dp0x" | clip
del "%~dp0x" /f /q
mshta vbscript:msgbox("Cheat Engine lua code template has been copied to clipboard."+vbCrLf+"Open Cheat Engine and paste them into Table -> Show Cheat Table Lua Script."+vbCrLf,48,"Success")(window.close)
exit /b
:AB
ECHO|set/p=[11;12H[40m[93m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[12;12H[103m[30m (B) Generate animation code with fields                  [40m 
ECHO|set/p=[13;12H[40m[93m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ 
timeout 1 /nobreak >nul
for /l %%a in (25 1 40) do (
set /a "cols=30+%%a*2"
call mode con:cols=%%cols%% lines=%%a >nul
)
echo|set/p=[2J[H[?25h[40m[97m
set Output=echo.ThreadList = {} ^>"x"
:Again
echo.Animation type: 
echo.1. Smooth
echo.2. Target
echo.3. Instantaneous
echo|set/p=^^^>
choice /C 123 /n
echo.
if "%errorlevel%" equ "1" (goto Smooth)
if "%errorlevel%" equ "2" (goto Target)
if "%errorlevel%" equ "3" (goto Instantaneous)
:Smooth
echo.Timing unit: 
echo.1. Tick
echo.2. Microseconds
echo|set/p=^^^>
choice /C 12 /n
echo.
if "%errorlevel%" equ "1" (set TimingUnit=Tick)
if "%errorlevel%" equ "2" (set TimingUnit=Time)
set "Input= "
set /P "Input=Start value: "
set "Input=!Input:^=^^!"
set "Input=!Input:"=^^^"!"
set "Input=!Input:&=^&!"
set "Input=!Input:<=^<!"
set "Input=!Input:>=^>!"
set "Input=!Input:|=^|!"
set "StartValue=!Input!"
set "Input= "
set /P "Input=End value: "
set "Input=!Input:^=^^!"
set "Input=!Input:"=^^^"!"
set "Input=!Input:&=^&!"
set "Input=!Input:<=^<!"
set "Input=!Input:>=^>!"
set "Input=!Input:|=^|!"
set "EndValue=!Input!"
set "Input= "
set /P "Input=Start time: "
set "Input=!Input:^=^^!"
set "Input=!Input:"=^^^"!"
set "Input=!Input:&=^&!"
set "Input=!Input:<=^<!"
set "Input=!Input:>=^>!"
set "Input=!Input:|=^|!"
set "StartTime=!Input!"
set "Input= "
set /P "Input=End time: "
set "Input=!Input:^=^^!"
set "Input=!Input:"=^^^"!"
set "Input=!Input:&=^&!"
set "Input=!Input:<=^<!"
set "Input=!Input:>=^>!"
set "Input=!Input:|=^|!"
set "EndTime=!Input!"
echo.Easing type: 
echo.1. Sine
echo.2. Quad
echo.3. Cubic
echo.4. Quart
echo.5. Quint
echo.6. Expo
echo.7. Circ
echo.8. Back
echo.9. Elastic
echo.0. Bounce
echo.A. Linear
echo|set/p=^^^>
choice /C 1234567890a /n
echo.
if "%errorlevel%" equ "1" (set EasingType=Sine)
if "%errorlevel%" equ "2" (set EasingType=Quad)
if "%errorlevel%" equ "3" (set EasingType=Cubic)
if "%errorlevel%" equ "4" (set EasingType=Quart)
if "%errorlevel%" equ "5" (set EasingType=Quint)
if "%errorlevel%" equ "6" (set EasingType=Expo)
if "%errorlevel%" equ "7" (set EasingType=Circ)
if "%errorlevel%" equ "8" (set EasingType=Back)
if "%errorlevel%" equ "9" (set EasingType=Elastic)
if "%errorlevel%" equ "10" (set EasingType=Bounce)
if "%errorlevel%" equ "11" (set EasingDirection=&set EasingType=Linear&goto LinearSmooth)
echo.Easing direction: 
echo.1. In
echo.2. Out
echo.3. In ^& Out
echo|set/p=^^^>
choice /C 123 /n
echo.
if "%errorlevel%" equ "1" (set EasingDirection=In)
if "%errorlevel%" equ "2" (set EasingDirection=Out)
if "%errorlevel%" equ "3" (set EasingDirection=InOut)
:LinearSmooth
echo.Animation target: 
echo.1. Volume
echo.2. PlaybackSpeed
echo.3. NoteSpeed
echo.4. Offset-X
echo.5. Offset-Y
echo.6. Zoom
echo|set/p=^^^>
choice /C 123456 /n
echo.
if "%errorlevel%" equ "1" (set AnimationTarget=SetVolume)
if "%errorlevel%" equ "2" (set AnimationTarget=SetPlaybackSpeed)
if "%errorlevel%" equ "3" (set AnimationTarget=SetNoteSpeed)
if "%errorlevel%" equ "4" (set AnimationTarget=SetOffsetX)
if "%errorlevel%" equ "5" (set AnimationTarget=SetOffsetY)
if "%errorlevel%" equ "6" (set AnimationTarget=SetZoom)
echo.Slicing: 
echo.1. Sliced
echo.2. Normal
echo|set/p=^^^>
choice /C 12 /n
echo.
if "%errorlevel%" equ "1" (goto SlicedSmooth)
if "%errorlevel%" equ "2" (goto UnslicedSmooth)
:SlicedSmooth
set "Input= "
set /P "Input=Spacing: "
set "Input=!Input:^=^^!"
set "Input=!Input:"=^^^"!"
set "Input=!Input:&=^&!"
set "Input=!Input:<=^<!"
set "Input=!Input:>=^>!"
set "Input=!Input:|=^|!"
set "Spacing=!Input!"
set "Output=!Output!^&echo.table.insert(ThreadList,SlicedSmoothAnimationBy!TimingUnit!(!StartValue!,!EndValue!,!StartTime!,!EndTime!,!Spacing!,Ease!EasingDirection!!EasingType!,!AnimationTarget!)) >>^"%~dp0x^""
goto Finish
:UnslicedSmooth
set "Output=!Output!^&echo.table.insert(ThreadList,SmoothAnimationBy!TimingUnit!(!StartValue!,!EndValue!,!StartTime!,!EndTime!,Ease!EasingDirection!!EasingType!,!AnimationTarget!)) >>^"%~dp0x^""
goto Finish
:Target
echo.Timing unit: 
echo.1. Tick
echo.2. Microseconds
echo|set/p=^^^>
choice /C 12 /n
echo.
if "%errorlevel%" equ "1" (set TimingUnit=Tick)
if "%errorlevel%" equ "2" (set TimingUnit=Time)
set "Input= "
set /P "Input=Target value: "
set "Input=!Input:^=^^!"
set "Input=!Input:"=^^^"!"
set "Input=!Input:&=^&!"
set "Input=!Input:<=^<!"
set "Input=!Input:>=^>!"
set "Input=!Input:|=^|!"
set "TargetValue=!Input!"
set "Input= "
set /P "Input=Start time: "
set "Input=!Input:^=^^!"
set "Input=!Input:"=^^^"!"
set "Input=!Input:&=^&!"
set "Input=!Input:<=^<!"
set "Input=!Input:>=^>!"
set "Input=!Input:|=^|!"
set "StartTime=!Input!"
set "Input= "
set /P "Input=End time: "
set "Input=!Input:^=^^!"
set "Input=!Input:"=^^^"!"
set "Input=!Input:&=^&!"
set "Input=!Input:<=^<!"
set "Input=!Input:>=^>!"
set "Input=!Input:|=^|!"
set "EndTime=!Input!"
echo.Easing type: 
echo.1. Sine
echo.2. Quad
echo.3. Cubic
echo.4. Quart
echo.5. Quint
echo.6. Expo
echo.7. Circ
echo.8. Back
echo.9. Elastic
echo.0. Bounce
echo.A. Linear
echo|set/p=^^^>
choice /C 1234567890a /n
echo.
if "%errorlevel%" equ "1" (set EasingType=Sine)
if "%errorlevel%" equ "2" (set EasingType=Quad)
if "%errorlevel%" equ "3" (set EasingType=Cubic)
if "%errorlevel%" equ "4" (set EasingType=Quart)
if "%errorlevel%" equ "5" (set EasingType=Quint)
if "%errorlevel%" equ "6" (set EasingType=Expo)
if "%errorlevel%" equ "7" (set EasingType=Circ)
if "%errorlevel%" equ "8" (set EasingType=Back)
if "%errorlevel%" equ "9" (set EasingType=Elastic)
if "%errorlevel%" equ "10" (set EasingType=Bounce)
if "%errorlevel%" equ "11" (set EasingDirection=&set EasingType=Linear&goto LinearTarget)
echo.Easing direction: 
echo.1. In
echo.2. Out
echo.3. In ^& Out
echo|set/p=^^^>
choice /C 123 /n
echo.
if "%errorlevel%" equ "1" (set EasingDirection=In)
if "%errorlevel%" equ "2" (set EasingDirection=Out)
if "%errorlevel%" equ "3" (set EasingDirection=InOut)
:LinearTarget
echo.Animation target: 
echo.1. Volume
echo.2. PlaybackSpeed
echo.3. NoteSpeed
echo.4. StartKey
echo.5. EndKey
echo.6. Offset-X
echo.7. Offset-Y
echo.8. Zoom
echo|set/p=^^^>
choice /C 12345678 /n
echo.
if "%errorlevel%" equ "1" (set AnimationTarget=SetVolume&set AnimationSource=GetVolume)
if "%errorlevel%" equ "2" (set AnimationTarget=SetPlaybackSpeed&set AnimationSource=GetPlaybackSpeed)
if "%errorlevel%" equ "3" (set AnimationTarget=SetNoteSpeed&set AnimationSource=GetNoteSpeed)
if "%errorlevel%" equ "4" (set AnimationTarget=SetOffsetX&set AnimationSource=StartKey)
if "%errorlevel%" equ "5" (set AnimationTarget=SetOffsetY&set AnimationSource=EndKey)
if "%errorlevel%" equ "6" (set AnimationTarget=SetOffsetX&set AnimationSource=GetOffsetX)
if "%errorlevel%" equ "7" (set AnimationTarget=SetOffsetY&set AnimationSource=GetOffsetY)
if "%errorlevel%" equ "8" (set AnimationTarget=SetZoom&set AnimationSource=GetZoom)
echo.Slicing: 
echo.1. Sliced
echo.2. Normal
echo|set/p=^^^>
choice /C 12 /n
echo.
if "%errorlevel%" equ "1" (goto SlicedTarget)
if "%errorlevel%" equ "2" (goto UnslicedTarget)
:SlicedTarget
set "Input= "
set /P "Input=Spacing: "
set "Input=!Input:^=^^!"
set "Input=!Input:"=^^^"!"
set "Input=!Input:&=^&!"
set "Input=!Input:<=^<!"
set "Input=!Input:>=^>!"
set "Input=!Input:|=^|!"
set "Spacing=!Input!"
set "Output=!Output!^&echo.table.insert(ThreadList,SlicedTargetAnimationBy!TimingUnit!(!AnimationSource!,!TargetValue!,!StartTime!,!EndTime!,!Spacing!,Ease!EasingDirection!!EasingType!,!AnimationTarget!)) >>^"%~dp0x^""
goto Finish
:UnslicedTarget
set "Output=!Output!^&echo.table.insert(ThreadList,TargetAnimationBy!TimingUnit!(!AnimationSource!,!TargetValue!,!StartTime!,!EndTime!,Ease!EasingDirection!!EasingType!,!AnimationTarget!)) >>^"%~dp0x^""
goto Finish
:Instantaneous
echo.Timing unit: 
echo.1. Tick
echo.2. Microseconds
echo|set/p=^^^>
choice /C 12 /n
echo.
if "%errorlevel%" equ "1" (set TimingUnit=Tick)
if "%errorlevel%" equ "2" (set TimingUnit=Time)
set "Input= "
set /P "Input=Value: "
set "Input=!Input:^=^^!"
set "Input=!Input:"=^^^"!"
set "Input=!Input:&=^&!"
set "Input=!Input:<=^<!"
set "Input=!Input:>=^>!"
set "Input=!Input:|=^|!"
set "Value=!Input!"
set "Input= "
set /P "Input=Time: "
set "Input=!Input:^=^^!"
set "Input=!Input:"=^^^"!"
set "Input=!Input:&=^&!"
set "Input=!Input:<=^<!"
set "Input=!Input:>=^>!"
set "Input=!Input:|=^|!"
set "Time=!Input!"
echo.Animation target: 
echo.1. Microseconds 
echo.2. Volume
echo.3. Mute
echo.4. PlaybackSpeed
echo.5. NoteSpeed
echo.6. StartKey
echo.7. EndKey
echo.8. KeyMode
echo.9. Offset-X
echo.0. Offset-Y
echo.A. Zoom
echo.B. Paused
echo.C. Keyboard
echo.D. VisualizePitchBends
echo.E. PhigrosMode
echo.F. ShorMarkers
echo.G. TickBased
echo.H. DisableUI
echo.I. LimitFPS
echo.J. Caption
echo|set/p=^^^>
choice /C 1234567890abcdefghij /n
echo.
if "%errorlevel%" equ "1" (set AnimationTarget=SetMicroseconds)
if "%errorlevel%" equ "2" (set AnimationTarget=SetVolume)
if "%errorlevel%" equ "3" (set AnimationTarget=SetMute)
if "%errorlevel%" equ "4" (set AnimationTarget=SetPlaybackSpeed)
if "%errorlevel%" equ "5" (set AnimationTarget=SetNoteSpeed)
if "%errorlevel%" equ "6" (set AnimationTarget=StartKey)
if "%errorlevel%" equ "7" (set AnimationTarget=EndKey)
if "%errorlevel%" equ "8" (set AnimationTarget=KeyMode)
if "%errorlevel%" equ "9" (set AnimationTarget=SetOffsetX)
if "%errorlevel%" equ "10" (set AnimationTarget=SetOffsetY)
if "%errorlevel%" equ "11" (set AnimationTarget=SetZoom)
if "%errorlevel%" equ "12" (set AnimationTarget=SetPaused)
if "%errorlevel%" equ "13" (set AnimationTarget=SetKeyboard)
if "%errorlevel%" equ "14" (set AnimationTarget=SetVisualizePitchBends)
if "%errorlevel%" equ "15" (set AnimationTarget=SetPhigrosMode)
if "%errorlevel%" equ "16" (set AnimationTarget=SetShowMarkers)
if "%errorlevel%" equ "17" (set AnimationTarget=SetTickBased)
if "%errorlevel%" equ "18" (set AnimationTarget=SetDisableUI)
if "%errorlevel%" equ "19" (set AnimationTarget=SetLimitFPS)
if "%errorlevel%" equ "20" (set AnimationTarget=SetCaption)
set "Output=!Output!^&echo.table.insert(ThreadList,InstantaneousAnimationBy!TimingUnit!(!Value!,!Time!,!AnimationTarget!)) >>^"%~dp0x^""
:Finish
echo|set/p=Do you want to add another animation? 
choice
if "%errorlevel%" equ "1" (goto Again)
if "%errorlevel%" equ "2" (goto Generate)
:Generate
set "Output=!Output!^&echo.RunThreads(ThreadList) >>^"%~dp0x^""
%Output%
type "%~dp0x" | clip
del "%~dp0x" /f /q
mshta vbscript:msgbox("The generated lua code has been copied to clipboard.",48,"Success")(window.close)
for /l %%a in (40 -1 25) do (
set /a "cols=30+%%a*2"
call mode con:cols=%%cols%% lines=%%a >nul
)
exit/b
:AC
ECHO|set/p=[14;12H[40m[93m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[15;12H[103m[30m (C) Advanced visualize pitch bend tricks tutorial        [40m 
ECHO|set/p=[16;12H[40m[93m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ 
timeout 1 /nobreak >nul
start "" "%~dp0PitchBendTutorial.rtf"
exit /b
:AD
ECHO|set/p=[17;12H[40m[93m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[18;12H[103m[30m (D) Cheat engine lua scripting tutorial                  [40m 
ECHO|set/p=[19;12H[40m[93m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ 
timeout 1 /nobreak >nul
start "" "%~dp0CheatEngineTutorial.rtf"
exit /b
:CA
ECHO|set/p=[8;12H[40m[93m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[9;12H[103m[30m (A) Generate color events from lyrics (Domino only^^^!)     [40m 
ECHO|set/p=[10;12H[40m[93m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ 
timeout 1 /nobreak >nul
for /l %%a in (25 1 40) do (
set /a "cols=30+%%a*2"
call mode con:cols=%%cols%% lines=%%a >nul
)
echo|set/p=[2J[H[?25h[40m[97m
"%~dp0mmfstuff.exe"
echo.
set "Input= "
set /P "InputMIDI=MIDI file path: "
set "Output=!InputMIDI!.colored.mid"
echo.
"%~dp0mmfstuff.exe" "!InputMIDI!" "!Output!"
echo.
pause
for /l %%a in (40 -1 25) do (
set /a "cols=30+%%a*2"
call mode con:cols=%%cols%% lines=%%a >nul
)
exit/b
:CB
ECHO|set/p=[11;12H[40m[93m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
ECHO|set/p=[12;12H[103m[30m (B) Generate color events from a color event script      [40m 
ECHO|set/p=[13;12H[40m[93m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ 
timeout 1 /nobreak >nul
for /l %%a in (25 1 40) do (
set /a "cols=30+%%a*2"
call mode con:cols=%%cols%% lines=%%a >nul
)
echo|set/p=[2J[H[?25h[40m[97m
"%~dp0easycolorevents.exe"
echo.
set "Input= "
set /P "InputMIDI=MIDI file path: "
set /P "InputCES=Color event script path: "
echo.
"%~dp0easycolorevents.exe" "!InputMIDI!" "!InputCES!"
echo.
pause
for /l %%a in (40 -1 25) do (
set /a "cols=30+%%a*2"
call mode con:cols=%%cols%% lines=%%a >nul
)
exit/b