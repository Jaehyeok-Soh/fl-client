@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul

echo 스마트 텍스처 변환기 (BC5 노멀 맵 + sRGB 자동 분류)
echo.

:: 1. texconv.exe 경로 확인
set "TEXCONV_PATH=%~dp0texconv.exe"
if not exist "!TEXCONV_PATH!" (
    echo 오류: texconv.exe를 찾을 수 없습니다!
    pause
    exit /b
)

set "TOTAL_COUNT=0"

:: 2. 하위 폴더 중 Material 폴더 탐색
for /d /r "%CD%" %%D in (Material) do (
    if exist "%%D\" (
        echo 작업 중: %%D
        pushd "%%D"
        
        for %%F in (*.png) do (
            set "FILE_NAME=%%~nF"
            
            :: 기본값: Linear (기타 데이터용)
            set "FORMAT=BC7_UNORM"
            set "TYPE=Linear"

            :: [1차 판정] 컬러/디퓨즈 태그가 있으면 sRGB로 설정
            echo !FILE_NAME! | findstr /i "_D _BC _TINT _DH" >nul
            if !errorlevel! == 0 (
                set "FORMAT=BC7_UNORM_SRGB"
                set "TYPE=sRGB"
            )

            :: [2차 판정] 노멀 맵 태그가 있으면 BC5_UNORM으로 덮어쓰기 (강력 방어)
            echo !FILE_NAME! | findstr /i "_N _NM _NRM _Normal" >nul
            if !errorlevel! == 0 (
                set "FORMAT=BC5_UNORM"
                set "TYPE=Normal(BC5)"
            )

            echo [!TYPE!] %%F -^> !FORMAT!
            
            :: 변환 실행
            "!TEXCONV_PATH!" -f !FORMAT! -m 0 -y "%%F" >nul
            set /a TOTAL_COUNT+=1
        )
        popd
        echo.
    )
)

echo 총 !TOTAL_COUNT!개의 에셋 변환 완료!
pause