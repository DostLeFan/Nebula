@echo off
setlocal

set PRESET=%1
set GENERATOR=%2

if "%PRESET%"=="" set PRESET=debug
if "%GENERATOR%"=="" set GENERATOR=MinGW Makefiles

cmake --preset %PRESET% -G "%GENERATOR%"
cmake --build --preset %PRESET%