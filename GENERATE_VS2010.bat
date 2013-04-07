@echo off

mkdir build
chdir build

mkdir msvc10
chdir msvc10

del CMakeCache.txt

cmake -G "Visual Studio 10" ../../

if %errorlevel% NEQ 0 goto error
goto end

:error
echo There was an error. Press any key to close.
pause >nul

:end
REM echo Press any key to close.
REM pause >nul
