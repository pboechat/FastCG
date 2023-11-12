@echo off

if "%NDKROOT%" == "" (
	echo Cannot find NDKROOT environment variable.
	exit /b -1
)

mkdir .vscode
copy /Y resources\Android\.vscode\settings.json .vscode\settings.json