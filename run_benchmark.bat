@echo off
chcp 65001 >nul
echo ============================================
echo   TRMM Performance Benchmark
echo ============================================
echo.

if not exist "build\Release\benchmark.exe" (
    echo [ERROR] benchmark.exe not found!
    echo Please run build.bat first.
    pause
    exit /b 1
)

echo [INFO] Starting benchmark...
echo This may take several minutes...
echo.

cd build\Release
benchmark.exe

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Benchmark failed!
    cd ..\..
    pause
    exit /b 1
)

cd ..\..
echo.
echo ============================================
echo   Benchmark completed!
echo ============================================
echo.
pause