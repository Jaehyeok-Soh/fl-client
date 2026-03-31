#powershell -ExecutionPolicy Bypass -File .\json_png2dds.ps1

Get-ChildItem -Path "." -Directory -Recurse -Filter "Material" | ForEach-Object {
    $materialFolder = $_.FullName
    Get-ChildItem -Path $materialFolder -Filter "*.json" | ForEach-Object {
        $jsonPath = $_.FullName
        $content = Get-Content -Path $jsonPath -Raw -Encoding UTF8
        $newContent = $content -replace '\.png"', '.dds"'
        if ($content -ne $newContent) {
            Set-Content -Path $jsonPath -Value $newContent -Encoding UTF8 -NoNewline
            Write-Host "¼öÁ¤: $jsonPath"
        }
    }
}