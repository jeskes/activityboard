@echo off
setlocal enabledelayedexpansion

set "SOURCE_DIR=%~dp0"
set "TARGET_DRIVE=D:"
set "TARGET_PATH=D:\"

echo ============================================================
echo deploy sd cards (core, sound, storygadget)
echo ============================================================
echo.

:: --------------------------------------------------------------
:: core
:: --------------------------------------------------------------

:process_core
echo process core card?
set /p "choice=[Y]es / [N]o (skip) / [C]ancel: "
if /i "%choice%"=="C" goto end
if /i "%choice%"=="N" (
    echo core skipped.
    goto process_sound
)
if /i "%choice%" neq "Y" goto process_core

:check_sd_core
if not exist "%TARGET_PATH%" (
    echo please insert core sd card in %TARGET_DRIVE%.
    pause
    goto check_sd_core
)

set /p "confirm=enter 'format' in order to format sd card: "
if "%confirm%"=="format" (
    format %TARGET_DRIVE% /Q /X /V:core
) else (
    echo continue without formatting sd card.
)

echo converting bitmaps...
cd /d "%SOURCE_DIR%core"
call convert-bitmaps.cmd
if %errorlevel% neq 0 (
    echo failed to convert bitmaps. continue?
    pause
)

xcopy "%SOURCE_DIR%core" "%TARGET_PATH%" /E /I /Y
echo successfuly copied core sd card.
echo.

:: --------------------------------------------------------------
:: sound
:: --------------------------------------------------------------

:process_sound
echo process sound card?
set /p "choice=[Y]es / [N]o (skip) / [C]ancel: "
if /i "%choice%"=="C" goto end
if /i "%choice%"=="N" (
    echo sound skipped.
    goto process_storygadget
)
if /i "%choice%" neq "Y" goto process_sound

:check_sd_sound
if not exist "%TARGET_PATH%" (
    echo please insert sound sd card in %TARGET_DRIVE%.
    pause
    goto check_sd_sound
)

set /p "confirm=enter 'format' in order to format sd card: "
if "%confirm%"=="format" (
    format %TARGET_DRIVE% /Q /X /V:sound
) else (
    echo continue without formatting sd card.
)

xcopy "%SOURCE_DIR%sound" "%TARGET_PATH%" /E /I /Y
echo successfuly copied sound sd card.
echo.

:: --------------------------------------------------------------
:: storygadget
:: --------------------------------------------------------------

:process_storygadget
echo process storygadget?
set /p "choice=[Y]es / [N]o (skip) / [C]ancel: "
if /i "%choice%"=="C" goto end
if /i "%choice%"=="N" (
    echo storygadget skipped.
    goto end
)
if /i "%choice%" neq "Y" goto process_storygadget

:check_sd_storygadget
if not exist "%TARGET_PATH%" (
    echo please insert storygadget sd card in %TARGET_DRIVE%.
    pause
    goto check_sd_storygadget
)

set /p "confirm=enter 'format' in order to format sd card: "
if "%confirm%"=="format" (
    format %TARGET_DRIVE% /Q /X /V:storygadget
) else (
    echo continue without formatting sd card.
)

xcopy "%SOURCE_DIR%storygadget" "%TARGET_PATH%" /E /I /Y
echo successfuly copied storygadget sd card.
echo.

:end
echo done.
pause
exit /b 0
