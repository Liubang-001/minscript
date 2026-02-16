@echo off
REM Windows构建和测试脚本

echo === MiniScript 构建和测试 ===
echo.

REM 构建项目
call build_windows.bat
if errorlevel 1 (
    echo 构建失败，退出测试
    exit /b 1
)

echo.
echo === 开始测试 ===
echo.

REM 运行测试
call test_runner.bat

echo.
echo === 构建和测试完成 ===