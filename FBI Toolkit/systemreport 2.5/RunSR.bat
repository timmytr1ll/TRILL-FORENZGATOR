@echo off
echo =============
echo System Report
echo =============
echo.
echo Start: %date%, um %time% Uhr.
echo.
echo Systemübersicht:
echo ================
Overview.exe 0
echo.
echo Betriebssystem:
echo ===============
OSWin.exe 0
echo.
echo Dial-Up Netzwerk:
echo =================
DUN.exe 0
echo.
echo Netzwerk-Informationen:
echo =======================
NetInfo.exe 0
echo.
echo BIOS-Informationen:
echo ===================
BIOS.exe 0
echo.
echo Prozessor-Informationen:
echo ========================
CPU.exe 0
echo.
echo Speicher-Informationen:
echo =======================
Memory.exe 0
echo.
echo Laufwerks- und Datenträger-Informationen:
echo =========================================
Drives.exe 0
echo.
echo Liste der zuletzt angeschlossenen Datenträger:
echo ==============================================
DevEnum.exe 0
echo.
echo Zugriffe auf Wechseldatenträger:
echo ================================
Streams.exe 0
echo.
echo Systemvariablen:
echo ================
Envir.exe 0
echo.
echo Benutzer-Informationen:
echo =======================
User.exe 0
echo.
echo Installierte Anwendungen:
echo =========================
InstApps.exe 0
echo.
echo Windows Updates:
echo ================
Updates.exe 0
echo.
echo Laufende Prozesse:
echo ==================
ProcList.exe 0 /X
echo.
echo Systemdienste:
echo ==============
Services.exe 0
echo.
echo Installierte Drucker:
echo =====================
Printers.exe 0
echo.
echo Zuletzt verwendete Objekte:
echo ===========================
LastObj.exe 0
echo.
echo Automatisch gestartete Programme:
echo =================================
AutoRun.exe 0
echo.
echo Microsoft Anwendungen:
echo ======================
MsApps.exe 0
echo.
echo Ende: %date%, um %time% Uhr.
