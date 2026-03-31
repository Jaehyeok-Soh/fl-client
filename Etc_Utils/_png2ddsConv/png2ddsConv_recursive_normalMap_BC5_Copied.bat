@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul

echo 스마트 텍스처 변환기 (원본 보호 및 Conved 미러링 지원)
echo.

:: 1. texconv.exe 경로 확인
set "TEXCONV_PATH=%~dp0texconv.exe"
if not exist "!TEXCONV_PATH!" (
    echo 오류: texconv.exe를 찾을 수 없습니다!
    pause
    exit /b
)

:: 루트 경로(스크립트 실행 위치) 및 최종 출력 폴더(Conved) 설정
set "ROOT_DIR=%CD%"
set "CONVED_DIR=%ROOT_DIR%\Conved"
set "TOTAL_COUNT=0"

:: 2. 하위 폴더 중 Material 폴더 탐색
for /d /r "%ROOT_DIR%" %%D in (Material) do (
    if exist "%%D\" (
        :: 원본 Material 폴더의 절대 경로
        set "SRC_DIR=%%D"
        
        :: [핵심] 원본 경로에서 루트 경로를 잘라내어 상대 경로를 구함
        :: 예: C:\Project\Assets\Material -> \Assets\Material
        set "REL_PATH=!SRC_DIR:%ROOT_DIR%=!"
        
        :: 결과물을 저장할 최종 아웃풋 경로 조합
        :: 예: C:\Project\Conved\Assets\Material
        set "OUT_DIR=!CONVED_DIR!!REL_PATH!"
        
        :: 해당 계층의 폴더가 Conved 안에 아직 없다면 똑같이 생성해 줌
        if not exist "!OUT_DIR!" (
            mkdir "!OUT_DIR!"
        )

        echo 작업 중: !REL_PATH!
        pushd "%%D"
        
        for %%F in (*.png) do (
            set "FILE_NAME=%%~nF"
            
            :: 기본값: Linear
            set "FORMAT=BC7_UNORM"
            set "TYPE=Linear"

            :: [1차 판정] 컬러/디퓨즈 태그
            echo !FILE_NAME! | findstr /i "_D _BC _TINT _DH" >nul
            if !errorlevel! == 0 (
                set "FORMAT=BC7_UNORM_SRGB"
                set "TYPE=sRGB"
            )

            :: [2차 판정] 노멀 맵 강력 방어
            echo !FILE_NAME! | findstr /i "_N _NM _NRM _Normal" >nul
            if !errorlevel! == 0 (
                set "FORMAT=BC5_UNORM"
                set "TYPE=Normal(BC5)"
            )

            echo [!TYPE!] %%F -^> !FORMAT!
            
            :: 변환 실행: -o 옵션을 사용하여 새로 만든 OUT_DIR 에 .dds 저장!
            "!TEXCONV_PATH!" -f !FORMAT! -m 0 -y -o "!OUT_DIR!" "%%F" >nul
            set /a TOTAL_COUNT+=1
        )
        popd
        echo.
    )
)

echo 총 !TOTAL_COUNT!개의 에셋 변환 및 저장 완료!
echo 결과물 확인 경로: %CONVED_DIR%
pause