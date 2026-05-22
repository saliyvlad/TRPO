@echo off
chcp 65001 >nul
echo ============================================
echo   Running TRMM Tests
echo ============================================
echo.

if not exist "build\Release\test_trmm.exe" (
    echo [ERROR] test_trmm.exe not found!
    echo Please run build.bat first.
    pause
    exit /b 1
)

cd build\Release
echo [INFO] Running tests...
echo.

test_trmm.exe

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Tests failed!
    cd ..\..
    pause
    exit /b 1
)

cd ..\..
echo.
echo ============================================
echo   All tests passed!
echo ============================================
echo.
pause