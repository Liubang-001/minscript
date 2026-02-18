@echo off
REM 编译示例扩展为动态库

echo === 编译示例扩展 ===
echo.

set CC=gcc
set CFLAGS=-Wall -Wextra -std=c99 -O2 -fPIC
set INCLUDE_DIR=include
set SRC_DIR=src

echo 编译示例扩展...
%CC% %CFLAGS% -I%INCLUDE_DIR% -shared -o example.dll %SRC_DIR%\ext\example_ext.c -L. -lminiscript -Wl,--out-implib,libexample.a

if errorlevel 1 (
    echo 编译失败！
    pause
    exit /b 1
)

echo.
echo === 编译成功！ ===
echo 生成的文件:
echo   - example.dll (动态库)
echo.
echo 使用方法:
echo   将 example.dll 放在 miniscript.exe 同一目录
echo   然后在脚本中使用: import example
echo.
