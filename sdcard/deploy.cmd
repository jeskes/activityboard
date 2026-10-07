@echo off
setlocal enabledelayedexpansion

:: Pfade definieren (aktuelles Verzeichnis des Skripts)
set "SOURCE_DIR=%~dp0"
set "TARGET_DRIVE=D:"
:: Das Zielverzeichnis für XCOPY braucht den Backslash
set "TARGET_PATH=D:\"

echo ============================================================
echo   SD-Karten Kopier-Skript (Direkt ins Hauptverzeichnis)
echo ============================================================
echo.

:: --------------------------------------------------------------
:: 1. Ordner: display
:: --------------------------------------------------------------
:process_display
echo [1/3] Ordner "display" verarbeiten?
set /p "choice=[J]a / [N]eien (ueberspringen) / [A]bbrechen: "
if /i "%choice%"=="A" goto end
if /i "%choice%"=="N" (
    echo "display" uebersprungen.
    goto process_sound
)
if /i "%choice%" neq "J" goto process_display

:: Schleife bis SD-Karte bereit ist
:check_sd_display
if not exist "%TARGET_PATH%" (
    echo.
    echo [!] FEHLER: SD-Karte in %TARGET_DRIVE% wurde nicht gefunden!
    echo Bitte legen Sie die SD-Karte fuer "display" ein.
    pause
    goto check_sd_display
)

echo.
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo !!! ACHTUNG: LAUFWERK %TARGET_DRIVE% WIRD BERREINIGT / FORMATIERT !!!
echo !!! ALLE DATEN AUF DER SD-KARTE GEHEN VERLOREN               !!!
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
set /p "confirm=Geben Sie zur Bestaetigung 'LOESCHEN' ein (Grossbuchstaben!): "
if "%confirm%"=="LOESCHEN" (
    echo Formatierung von %TARGET_DRIVE% wird gestartet...
    :: /Q = Schnellformatierung, /X = Erzwingt das Bereitstellenaufheben (Auswerfen aktiver Griffe)
    format %TARGET_DRIVE% /Q /X /V:Display
) else (
    echo Formatierung ABGEBROCHEN. Fahre ohne Loeschen fort.
)
echo.

echo Fuehre "convert-bitmaps.cmd" aus...
cd /d "%SOURCE_DIR%display"
call convert-bitmaps.cmd
if %errorlevel% neq 0 (
    echo WARNUNG: "convert-bitmaps.cmd" wurde mit Fehlern beendet.
    echo Trotzdem fortfahren?
    pause
)

echo Kopiere Inhalte aus "display" direkt nach %TARGET_PATH%...
xcopy "%SOURCE_DIR%display" "%TARGET_PATH%" /E /I /Y
echo "display" erfolgreich kopiert!
echo.
echo ============================================================
echo.

:: --------------------------------------------------------------
:: 2. Ordner: sound
:: --------------------------------------------------------------
:process_sound
echo [2/3] Ordner "sound" verarbeiten?
set /p "choice=[J]a / [N]eien (ueberspringen) / [A]bbrechen: "
if /i "%choice%"=="A" goto end
if /i "%choice%"=="N" (
    echo "sound" uebersprungen.
    goto process_storygadget
)
if /i "%choice%" neq "J" goto process_sound

:check_sd_sound
if not exist "%TARGET_PATH%" (
    echo.
    echo [!] FEHLER: SD-Karte in %TARGET_DRIVE% wurde nicht gefunden!
    echo Bitte legen Sie die SD-Karte fuer "sound" ein.
    pause
    goto check_sd_sound
)

echo.
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo !!! ACHTUNG: LAUFWERK %TARGET_DRIVE% WIRD BERREINIGT / FORMATIERT !!!
echo !!! ALLE DATEN AUF DER SD-KARTE GEHEN VERLOREN               !!!
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
set /p "confirm=Geben Sie zur Bestaetigung 'LOESCHEN' ein (Grossbuchstaben!): "
if "%confirm%"=="LOESCHEN" (
    echo Formatierung von %TARGET_DRIVE% wird gestartet...
    format %TARGET_DRIVE% /Q /X V:Sound
) else (
    echo Formatierung ABGEBROCHEN. Fahre ohne Loeschen fort.
)
echo.

echo Kopiere Inhalte aus "sound" direkt nach %TARGET_PATH%...
xcopy "%SOURCE_DIR%sound" "%TARGET_PATH%" /E /I /Y
echo "sound" erfolgreich kopiert!
echo.
echo ============================================================
echo.

:: --------------------------------------------------------------
:: 3. Ordner: storygadget
:: --------------------------------------------------------------
:process_storygadget
echo [3/3] Ordner "storygadget" verarbeiten?
set /p "choice=[J]a / [N]eien (ueberspringen) / [A]bbrechen: "
if /i "%choice%"=="A" goto end
if /i "%choice%"=="N" (
    echo "storygadget" uebersprungen.
    goto end
)
if /i "%choice%" neq "J" goto process_storygadget

:check_sd_storygadget
if not exist "%TARGET_PATH%" (
    echo.
    echo [!] FEHLER: SD-Karte in %TARGET_DRIVE% wurde nicht gefunden!
    echo Bitte legen Sie die SD-Karte fuer "storygadget" ein.
    pause
    goto check_sd_storygadget
)

echo.
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo !!! ACHTUNG: LAUFWERK %TARGET_DRIVE% WIRD BERREINIGT / FORMATIERT !!!
echo !!! ALLE DATEN AUF DER SD-KARTE GEHEN VERLOREN               !!!
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
set /p "confirm=Geben Sie zur Bestaetigung 'LOESCHEN' ein (Grossbuchstaben!): "
if "%confirm%"=="LOESCHEN" (
    echo Formatierung von %TARGET_DRIVE% wird gestartet...
    format %TARGET_DRIVE% /Q /X V:StoryGadget
) else (
    echo Formatierung ABGEBROCHEN. Fahre ohne Loeschen fort.
)
echo.

echo Kopiere Inhalte aus "storygadget" direkt nach %TARGET_PATH%...
xcopy "%SOURCE_DIR%storygadget" "%TARGET_PATH%" /E /I /Y
echo "storygadget" erfolgreich kopiert!
echo.
echo ============================================================
echo.

:end
echo FERTIG! Vorgang beendet.
pause
exit /b 0
