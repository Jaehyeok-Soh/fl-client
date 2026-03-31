@echo off
setlocal enabledelayedexpansion

echo 스마트 텍스처 변환기 (sRGB 자동 분류)
echo.

:: 폴더 내의 모든 png 파일을 하나씩 꺼내서 검사합니다.
for %%F in (*.png) do (
    set "FILE_NAME=%%~nF"
    
    :: 기본값은 노멀, 러프니스 등을 위한 Linear 포맷으로 설정
    set "FORMAT=BC7_UNORM"
    set "TYPE=Linear"

    :: 파일명에 특정 태그(_D, _BC, _TINT, _DH)가 있는지 대소문자 무시(/i)하고 검사
    echo !FILE_NAME! | findstr /i "_D _BC _TINT _DH" >nul
    if !errorlevel! == 0 (
        :: 태그가 발견되면 sRGB 포맷으로 덮어씌움!
        set "FORMAT=BC7_UNORM_SRGB"
        set "TYPE=sRGB"
    )

    :: 현재 변환 중인 파일의 상태를 콘솔에 예쁘게 출력
    echo [!TYPE!] %%F -^> !FORMAT!
    
    :: 실제로 texconv 실행 (콘솔 출력이 너무 지저분해지지 않게 >nul 처리)
    texconv.exe -f !FORMAT! -m 0 -y "%%F" >nul
)

echo.
echo 모든 텍스처가 알맞은 색상 공간으로 변환되었습니다!
pause