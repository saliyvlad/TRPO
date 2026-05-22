@echo off
chcp 65001 >nul
echo ============================================
echo   Upload TRPO Project to GitHub
echo ============================================
echo.

REM Проверка наличия Git
git --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Git не установлен!
    echo Скачайте с https://git-scm.com/
    pause
    exit /b 1
)

REM Инициализация
if not exist .git (
    echo [INFO] Initializing Git repository...
    git init
    git branch -M main
)

REM Ваша ссылка на репозиторий
set REPO_URL=https://github.com/saliyvlad/TRPO.git

echo [INFO] Setting remote origin...
git remote remove origin 2>nul
git remote add origin %REPO_URL%

echo [INFO] Adding files...
git add .

echo [INFO] Committing changes...
git commit -m "Final TRMM implementation: BLAS Level 3, OpenMP parallelization, benchmarks"

echo.
echo [INFO] Uploading to GitHub...
echo ВНИМАНИЕ: Может открыться браузер для авторизации в GitHub.
echo.

git push -u origin main

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Upload failed!
    echo Проверьте: 1) интернет 2) доступ к репозиторию 3) авторизацию
    pause
    exit /b 1
)

echo.
echo ============================================
echo   SUCCESS! Project uploaded to:
echo   %REPO_URL%
echo ============================================
pause