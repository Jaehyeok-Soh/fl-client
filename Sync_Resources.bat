@echo off
chcp 65001 >nul
setlocal

cd /d "%~dp0"

echo ================================
echo  Drive - Local Sync (Resources)
echo ================================
echo.

REM 1) Prefer Python Launcher (py)
py -3 --version >nul 2>&1
if %errorlevel%==0 (
    echo [OK] Using: py -3
    py -3 "%~dp0ResourceCopy.py"
    goto :end
)

REM 2) Fallback to python
python --version >nul 2>&1
if %errorlevel%==0 (
    echo [OK] Using: python
    python "%~dp0ResourceCopy.py"
    goto :end
)

echo [ERROR] Python is not installed or not in PATH.
echo - Install Python 3 (recommended: python.org) and enable "Add to PATH"
echo - Then run again.
echo.

:end
echo.
pause
endlocal