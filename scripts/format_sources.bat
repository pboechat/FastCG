@echo off
setlocal EnableExtensions EnableDelayedExpansion

:: Format C/C++ sources with clang-format.
:: Usage:
::   scripts\format_source.bat                 -- format default dirs (FastCG, examples)
::   scripts\format_source.bat --staged        -- format only staged files, then re-stage
::   scripts\format_source.bat --check         -- CI: fail if formatting would change files
::   scripts\format_source.bat [files...]      -- format only the given files

:: Config / defaults
if "%CLANG_FORMAT%"=="" set "CLANG_FORMAT=clang-format"
set "CHECK=0"
set "STAGED=0"
set "DEFAULT_DIRS=FastCG;examples"

:: Resolve repo root (prefer Git, fallback to script-dir/..)
set "SCRIPT_DIR=%~dp0"
for /f "delims=" %%R in ('git rev-parse --show-toplevel 2^>NUL') do set "REPO_ROOT=%%R"
if not defined REPO_ROOT for %%# in ("%SCRIPT_DIR%..") do set "REPO_ROOT=%%~f#"
pushd "%REPO_ROOT%" >NUL

:: Parse args
set "FILES_GIVEN=0"
for %%A in (%*) do (
  set "ARG=%%~A"
  if /I "!ARG!"=="--check" (
    set "CHECK=1"
  ) else if /I "!ARG!"=="--staged" (
    set "STAGED=1"
  ) else if /I "!ARG!"=="--help" (
    call :HELP & exit /b 0
  ) else if /I "!ARG!"=="-h" (
    call :HELP & exit /b 0
  ) else (
    set "FILES_GIVEN=1"
    call :MAYBE_ADD_FILE "%%~A"
  )
)

:: Ensure clang-format is available
where "%CLANG_FORMAT%" >NUL 2>&1
if errorlevel 1 (
  >&2 echo error: "%CLANG_FORMAT%" not found in PATH
  exit /b 127
)

:: If no files were passed, collect from staged or from default dirs
if "%FILES_GIVEN%"=="0" (
  if "%STAGED%"=="1" (
    for /f "delims=" %%F in ('git diff --cached --name-only --diff-filter=ACMR') do (
      set "P=%%~F"
      set "P=!P:/=\!"
      call :CONSIDER_STAGED "!P!"
    )
  ) else (
    for %%D in (%DEFAULT_DIRS:;= %) do (
      if exist "%%D" (
        for /r "%%D" %%F in (*.c)   do @call :MAYBE_ADD_FILE "%%~fF"
        for /r "%%D" %%F in (*.cc)  do @call :MAYBE_ADD_FILE "%%~fF"
        for /r "%%D" %%F in (*.cpp) do @call :MAYBE_ADD_FILE "%%~fF"
        for /r "%%D" %%F in (*.cxx) do @call :MAYBE_ADD_FILE "%%~fF"
        for /r "%%D" %%F in (*.h)   do @call :MAYBE_ADD_FILE "%%~fF"
        for /r "%%D" %%F in (*.hh)  do @call :MAYBE_ADD_FILE "%%~fF"
        for /r "%%D" %%F in (*.hpp) do @call :MAYBE_ADD_FILE "%%~fF"
        for /r "%%D" %%F in (*.inc) do @call :MAYBE_ADD_FILE "%%~fF"
      )
    )
  )
)

:: Dedup list
if not defined __FILELIST set "__FILELIST=%TEMP%\format_files_%RANDOM%.lst"
if not exist "%__FILELIST%" (
  echo No files to format.
  popd >NUL
  exit /b 0
)
set "__DEDUP=%TEMP%\format_files_%RANDOM%_dedup.lst"
sort /unique "%__FILELIST%" > "%__DEDUP%"
move /y "%__DEDUP%" "%__FILELIST%" >NUL

:: Count files
set /a COUNT=0
for /f "usebackq delims=" %%L in ("%__FILELIST%") do set /a COUNT+=1
if %COUNT%==0 (
  echo No files to format.
  del "%__FILELIST%" >NUL 2>&1
  popd >NUL
  exit /b 0
)

if "%CHECK%"=="1" (
  call :HAS_DRYRUN
  set "FAILED=0"
  if "%HAVE_DRYRUN%"=="1" (
    for /f "usebackq delims=" %%F in ("%__FILELIST%") do (
      "%CLANG_FORMAT%" --dry-run -Werror "%%~F" >NUL 2>&1
      if errorlevel 1 (
        echo Would reformat: %%F
        set "FAILED=1"
      )
    )
  ) else (
    for /f "usebackq delims=" %%F in ("%__FILELIST%") do (
      set "__TMP=%TEMP%\cf_%RANDOM%.tmp"
      "%CLANG_FORMAT%" "%%~F" > "!__TMP!"
      fc /b "%%~F" "!__TMP!" >NUL
      if errorlevel 1 (
        echo Would reformat: %%F
        set "FAILED=1"
      )
      del "!__TMP!" >NUL 2>&1
    )
  )
  if "%FAILED%"=="1" (
    echo Formatting required. Run: scripts\format_source.bat
    del "%__FILELIST%" >NUL 2>&1
    popd >NUL
    exit /b 2
  ) else (
    echo Formatting OK.
    del "%__FILELIST%" >NUL 2>&1
    popd >NUL
    exit /b 0
  )
) else (
  echo Formatting %COUNT% file(s)...
  for /f "usebackq delims=" %%F in ("%__FILELIST%") do "%CLANG_FORMAT%" -i "%%~F"
  echo Formatting done.
  if "%STAGED%"=="1" (
    for /f "usebackq delims=" %%F in ("%__FILELIST%") do git add -- "%%~F"
  )
  del "%__FILELIST%" >NUL 2>&1
  popd >NUL
  exit /b 0
)

:: ---------------- helpers ----------------

:HELP
echo.
echo Format C/C++ sources with clang-format.
echo   format_source.bat               ^(format default dirs: FastCG, examples^)
echo   format_source.bat --staged      ^(only staged files; re-stages after fix^)
echo   format_source.bat --check       ^(fail if any file would be reformatted^)
echo   format_source.bat [files...]    ^(specific files^)
echo.
exit /b 0

:MAYBE_ADD_FILE
set "P=%~1"
if not exist "%P%" exit /b 0
call :MATCH_EXT "%P%"
if errorlevel 1 exit /b 0
if not defined __FILELIST set "__FILELIST=%TEMP%\format_files_%RANDOM%.lst"
>>"%__FILELIST%" echo %P%
exit /b 0

:CONSIDER_STAGED
set "S=%~1"
call :MATCH_EXT "%S%"
if errorlevel 1 exit /b 0
set "S=%S:/=\%"
set "PREFIX7=!S:~0,7!"
set "PREFIX9=!S:~0,9!"
if /I "!PREFIX7!"=="FastCG\"  (>>"%__FILELIST%" echo %S% & exit /b 0)
if /I "!PREFIX9!"=="examples\" (>>"%__FILELIST%" echo %S% & exit /b 0)
exit /b 0

:MATCH_EXT
set "EXT=%~x1"
for %%E in (.c .cc .cpp .cxx .h .hh .hpp .inc) do (
  if /I "%%E"=="%EXT%" exit /b 0
)
exit /b 1

:HAS_DRYRUN
"%CLANG_FORMAT%" --help 2>&1 | findstr /c:"--dry-run" >NUL
if errorlevel 1 ( set "HAVE_DRYRUN=0" ) else ( set "HAVE_DRYRUN=1" )
exit /b 0
