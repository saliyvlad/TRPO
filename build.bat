@echo off
chcp 65001 >nul
echo ============================================
echo   TRMM Project Build Script (Clean Build)
echo ============================================
echo.

REM Принудительно удаляем старую папку сборки
if exist build (
    echo [INFO] Cleaning old build directory...
    rmdir /s /q build
)

if not exist "C:\vcpkg\vcpkg.exe" (
    echo [ERROR] vcpkg not found at C:\vcpkg
    pause
    exit /b 1
)

mkdir build
cd build

echo [INFO] Configuring CMake...
cmake -G "Visual Studio 17 2022" ^
      -A x64 ^
      -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ^
      -DCMAKE_BUILD_TYPE=Release ^
      ..

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed!
    cd ..
    pause
    exit /b 1
)

echo.
echo [INFO] Building project...
cmake --build . --config Release --parallel

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed!
    cd ..
    pause
    exit /b 1
)

echo.
echo ============================================
echo   Build completed successfully!
echo   Executables: build\Release\
echo ============================================
cd ..
pause