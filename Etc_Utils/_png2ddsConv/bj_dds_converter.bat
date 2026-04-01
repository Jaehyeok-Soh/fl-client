@echo off
setlocal EnableExtensions EnableDelayedExpansion

REM =========================================
REM 사용자 설정
REM =========================================
set "NVTT=C:\Program Files\NVIDIA Corporation\NVIDIA Texture Tools\nvtt_export.exe"
set "ROOT=C:\Users\PC\Documents\FinalProject\Etc_Utils"

REM 0 = 기존 DDS 있으면 스킵
REM 1 = 기존 DDS 있어도 덮어쓰기
set "OVERWRITE=0"

REM 테스트용 제한
REM 0 = 전체 변환
REM 예: 10 = 처음 10개만 변환
set "LIMIT=0"

REM 로그 파일
set "LOGFILE=%~dp0png2dds_log.txt"
if exist "%LOGFILE%" del /f /q "%LOGFILE%"

REM =========================================
REM 유효성 검사
REM =========================================
if not exist "%NVTT%" (
    echo [ERROR] nvtt_export.exe를 찾을 수 없습니다.
    echo         %NVTT%
    pause
    exit /b 1
)

if not exist "%ROOT%" (
    echo [ERROR] ROOT 경로를 찾을 수 없습니다.
    echo         %ROOT%
    pause
    exit /b 1
)

set /a FOUND=0
set /a DONE=0
set /a FAIL=0
set /a SKIP=0

echo [START] ROOT=%ROOT%
echo [START] ROOT=%ROOT% > "%LOGFILE%"
echo.>> "%LOGFILE%"

REM =========================================
REM 재귀 탐색 후 직접 변환
REM =========================================
for /R "%ROOT%" %%F in (*.png) do (
    set "BASENAME=%%~nF"
    set "MATCH="

    REM 기본 Diffuse: *_D
    echo(!BASENAME!| findstr /R /I "_D$" >nul && set "MATCH=1"

    REM 필요 시 확장:
    REM echo(!BASENAME!| findstr /R /I "_BC$" >nul && set "MATCH=1"
    REM echo(!BASENAME!| findstr /R /I "_DH$" >nul && set "MATCH=1"
    REM echo(!BASENAME!| findstr /R /I "_TINT$" >nul && set "MATCH=1"

    if defined MATCH (
        set /a FOUND+=1

        if not "%LIMIT%"=="0" (
            if !FOUND! GTR %LIMIT% goto :AFTER_LOOP
        )

        set "INFILE=%%~fF"
        set "OUTFILE=%%~dpnF.dds"

        if "%OVERWRITE%"=="0" if exist "!OUTFILE!" (
            echo [SKIP] !OUTFILE!
            echo [SKIP] !OUTFILE!>> "%LOGFILE%"
            set /a SKIP+=1
        ) else (
            echo [RUN ] !INFILE!
            echo [RUN ] !INFILE!>> "%LOGFILE%"

            "%NVTT%" ^
              "!INFILE!" ^
              --format bc7 ^
              --export-transfer-function srgb ^
              --quality production ^
              --mips ^
              --mip-filter box ^
              --min-mip-size 1 ^
              --output "!OUTFILE!"

            if exist "!OUTFILE!" (
                echo [ OK ] !OUTFILE!
                echo [ OK ] !OUTFILE!>> "%LOGFILE%"
                set /a DONE+=1
            ) else (
                echo [FAIL] !OUTFILE!
                echo [FAIL] !OUTFILE!>> "%LOGFILE%"
                set /a FAIL+=1
            )

            echo.>> "%LOGFILE%"
        )
    )
)

:AFTER_LOOP
echo.
echo [INFO] matched=%FOUND% success=%DONE% fail=%FAIL% skip=%SKIP%
echo [INFO] matched=%FOUND% success=%DONE% fail=%FAIL% skip=%SKIP%>> "%LOGFILE%"
echo [INFO] log=%LOGFILE%
pause
exit /b 0