@echo off
echo 모든 PNG를 sRGB(BC7) 포맷으로 일괄 변환합니다...
echo.

:: -f BC7_UNORM_SRGB : 모든 텍스처를 sRGB 색상 공간이 적용된 BC7으로 압축
:: -m 0 : 밉맵(Mipmap)을 최소 크기까지 자동 생성
:: -y : 기존 dds 파일이 있으면 묻지 않고 덮어쓰기
texconv.exe -f BC7_UNORM_SRGB -m 0 -y *.png

echo.
echo 모든 텍스처의 sRGB 변환 및 저장이 완료되었습니다!
pause