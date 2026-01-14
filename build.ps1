# ------------------------------
# build.ps1 - сборка WinHBlocker.exe через MinGW-w64 + Clang
# ------------------------------

$sourceFile = "WinHBlocker.cpp"
$outputFile = "WinHBlocker.exe"

# Путь к MinGW-w64 (замени на свой, если отличается)
$MinGWPath = "C:\msys64\mingw64\bin"

# Проверка clang++
if (-not (Test-Path (Join-Path $MinGWPath "clang++.exe"))) {
    Write-Error "clang++ для MinGW-w64 не найден в $MinGWPath"
    exit 1
}

# Пути к MinGW include/lib
$IncludePath = "$MinGWPath\..\..\x86_64-w64-mingw32\include"
$LibPath = "$MinGWPath\..\..\x86_64-w64-mingw32\lib"

# Компиляция
Write-Host "Компиляция $sourceFile через MinGW-w64 clang..."

$clangArgs = @(
    $sourceFile,
    "-std=c++20",
    "-O2",
    "-municode",
    "-I`"$IncludePath`"",
    "-L`"$LibPath`"",
    "-luser32",
    "-lgdi32",
    "-lole32",
    "-lshell32",
    "-Wl,--subsystem,windows",
    "-o", $outputFile
)

# Запуск clang++
$process = Start-Process -FilePath (Join-Path $MinGWPath "clang++.exe") -ArgumentList $clangArgs -NoNewWindow -Wait -PassThru

if ($process.ExitCode -eq 0 -and (Test-Path $outputFile)) {
    Write-Host "Сборка завершена успешно! Файл готов: $outputFile"
} else {
    Write-Error "Сборка не удалась. Проверьте ошибки компилятора."
}
