@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul

echo 스마트 텍스처 변환기 (경로 꼬임 방지 안전 모드)
echo.

:: 1. texconv.exe 절대 경로 확보
set "TEXCONV_PATH=%~dp0texconv.exe"
if not exist "!TEXCONV_PATH!" (
    echo 오류: texconv.exe 파일을 찾을 수 없습니다!
    pause
    exit /b
)

set "TOTAL_COUNT=0"

:: 2. Material 폴더 탐색
for /d /r "%CD%" %%D in (Material) do (
    if exist "%%D\" (
        echo 탐색 중: %%D
        
        :: [핵심 해결책] 해당 Material 폴더로 아예 경로를 이동해 버립니다!
        pushd "%%D"
        
        :: 3. 이동한 폴더 안에서 png 파일만 찾아서 변환
        for %%F in (*.png) do (
            set "FILE_NAME=%%~nF"
            set "FORMAT=BC7_UNORM"
            set "TYPE=Linear"

            echo !FILE_NAME! | findstr /i "_D _BC _TINT _DH" >nul
            if !errorlevel! == 0 (
                set "FORMAT=BC7_UNORM_SRGB"
                set "TYPE=sRGB"
            )

            echo [!TYPE!] %%F -^> !FORMAT!
            
            :: 현재 폴더 안에 있으므로 절대 경로(-o) 없이 깔끔하게 실행
            "!TEXCONV_PATH!" -f !FORMAT! -m 0 -y "%%F" >nul
            
            set /a TOTAL_COUNT+=1
        )
        
        :: 작업이 끝났으니 원래 있던 밖의 경로로 다시 빠져나옵니다.
        popd
        echo.
    )
)

if !TOTAL_COUNT! == 0 (
    echo 변환할 .png 파일이 없거나 Material 폴더를 찾지 못했습니다.
) else (
    echo 총 !TOTAL_COUNT!개의 텍스처가 성공적으로 변환 및 저장되었습니다!
)

pause