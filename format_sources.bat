@echo off
setlocal enabledelayedexpansion

set "ROOT_DIR=%cd%"

rem format files in specified directories
for %%d in (FastCG samples) do (
	call :format_dir "%ROOT_DIR%\%%d"
)

echo All sources have been formatted
endlocal

goto :eof


:format_dir
for /r %1 %%f in (*.cpp *.h *.inc) do (
	echo Formatting %%f
	clang-format -i "%%f"
)
goto :eof