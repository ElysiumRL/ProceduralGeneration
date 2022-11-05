@echo off
DEL /q/s *.sln
DEL /q/s .vs
DEL /q/s Binaries
DEL /q/s DerivedDataCache
DEL /q/s Intermediate
DEL /q/s Saved

rmdir /q/s .vs
rmdir /q/s Binaries
rmdir /q/s DerivedDataCache
rmdir /q/s Intermediate
rmdir /q/s Saved

set path=""

@echo on
for /F "delims=" %%I in ('dir /s /b *.uproject') do (path="%%I")
start /b /d "C:\Program Files\Epic Games\UE_5.0\Engine\Binaries\DotNET\UnrealBuildTool\" UnrealBuildTool.exe -projectfiles -project=%path% -game -rocket -progress