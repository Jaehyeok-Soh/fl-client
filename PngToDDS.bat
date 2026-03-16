@echo off
setlocal EnableExtensions EnableDelayedExpansion

rem =========================================================
rem Compressonator CLI 경로
rem 1) 이 배치와 같은 폴더에 CompressonatorCLI.exe가 있거나
rem 2) PATH에 등록되어 있다고 가정
rem =========================================================
if exist ".\CompressonatorCLI.exe" (
    set "CMP=.\CompressonatorCLI.exe"
) else (
    set "CMP=CompressonatorCLI.exe"
)

where /q "%CMP%"
if errorlevel 1 (
    echo [ERROR] CompressonatorCLI.exe 를 찾을 수 없습니다.
    echo         배치 파일과 같은 폴더에 두거나 PATH에 등록해 주세요.
    pause
    exit /b 1
)

rem =========================================================
rem 공통 옵션
rem -EncodeWith CPU : GPU 제약 피하려고 CPU 사용
rem -mipsize 1      : 가능한 전체 mip chain 생성(1x1까지)
rem =========================================================
set "COMMON_OPTS=-EncodeWith CPU -mipsize 1"

set /a COUNT_OK=0
set /a COUNT_FAIL=0

rem 현재 폴더만 처리, 하위 폴더는 안 돎
for /f "delims=" %%F in ('dir /b /a-d "*.png" "*.tga" "*.bmp" "*.jpg" "*.jpeg" 2^>nul') do (
    set "SRC=%%~fF"
    set "NAME=%%~nF"
    set "DST=%%~dpnF.dds"

    rem -----------------------------------------------------
    rem 기본값
    rem -----------------------------------------------------
    set "FMT=BC7"
    set "TYPE=DEFAULT"

    rem -----------------------------------------------------
    rem 우선순위
    rem 1) _N       -> Normal
    rem 2) _M       -> AO/Rough/Metal packed
    rem 3) _E       -> Emissive
    rem 4) _DH/_BC/_TINT/_D -> sRGB color
    rem 5) 그 외 기본 BC7
    rem -----------------------------------------------------
    if /I not "!NAME!"=="!NAME:_N=!" (
        set "FMT=BC5"
        set "TYPE=NORMAL"
    ) else if /I not "!NAME!"=="!NAME:_M=!" (
        set "FMT=BC7"
        set "TYPE=MASK_ARM"
    ) else if /I not "!NAME!"=="!NAME:_E=!" (
        set "FMT=BC7"
        set "TYPE=EMISSIVE"
    ) else (
        set "IS_SRGB=0"

        if /I not "!NAME!"=="!NAME:_DH=!"   set "IS_SRGB=1"
        if /I not "!NAME!"=="!NAME:_BC=!"   set "IS_SRGB=1"
        if /I not "!NAME!"=="!NAME:_TINT=!" set "IS_SRGB=1"
        if /I not "!NAME!"=="!NAME:_D=!"    set "IS_SRGB=1"

        if "!IS_SRGB!"=="1" (
            set "FMT=BC7"
            set "TYPE=SRGB_COLOR"
        )
    )

    if exist "!DST!" del /f /q "!DST!" >nul 2>&1

    echo [!TYPE!] "%%F" ^> "%%~nF.dds"  (!FMT!, mip=full)

    "%CMP%" %COMMON_OPTS% -fd !FMT! "!SRC!" "!DST!"
    if errorlevel 1 (
        echo     [FAIL] %%F
        set /a COUNT_FAIL+=1
    ) else (
        set /a COUNT_OK+=1
    )
)

echo.
echo ==========================================
echo Done.
echo Success : !COUNT_OK!
echo Failed  : !COUNT_FAIL!
echo ==========================================
pause