@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul

echo 텍스처 청소기: Material 폴더 내 .png 일괄 삭제
echo.
echo [경고] 이 스크립트는 하위 폴더를 모두 뒤져 'Material' 폴더 안의 .png 파일을 영구 삭제합니다!
echo .dds 변환과 JSON 파일 업데이트가 모두 정상적으로 완료되었는지 꼭 확인하세요.
echo.
echo 진행하시려면 아무 키나 누르시고, 취소하려면 창을 닫아주세요.
pause >nul

echo.
echo 불필요한 원본 .png 파일 삭제를 시작합니다...
echo.

set "FOLDER_COUNT=0"

:: 1. 현재 디렉토리부터 하위 폴더를 순회하며 이름이 'Material'인 폴더를 찾습니다.
for /d /r "%CD%" %%D in (Material) do (
    if exist "%%D\" (
        :: 2. 폴더 안에 지울 png 파일이 진짜로 있는지 먼저 확인합니다.
        if exist "%%D\*.png" (
            echo 청소 완료: %%D
            
            :: 3. /q 옵션(Quiet)을 줘서 묻지 않고 해당 폴더 안의 모든 png를 즉시 삭제합니다.
            del /q "%%D\*.png"
            
            set /a FOLDER_COUNT+=1
        )
    )
)

echo.
if !FOLDER_COUNT! == 0 (
    echo 지울 .png 파일이 이미 없거나 Material 폴더를 찾지 못했습니다.
) else (
    echo 총 !FOLDER_COUNT!개의 Material 폴더에서 원본 .png 파일을 싹 비웠습니다!
)

pause