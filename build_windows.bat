@echo off
REM Windows编译脚本

echo === MiniScript Windows 编译脚本 ===
echo.

REM 检查是否安装了GCC
gcc --version >nul 2>&1
if errorlevel 1 (
    echo 错误: 找不到GCC编译器
    echo 请安装MinGW-w64或TDM-GCC
    echo 下载地址: https://www.mingw-w64.org/downloads/
    pause
    exit /b 1
)

echo 检测到GCC编译器，开始编译...
echo.

REM 设置编译选项
set CC=gcc
set CFLAGS=-Wall -Wextra -std=c99 -O2
set LDFLAGS=-lm
set BUILD_DIR=build
set INCLUDE_DIR=include
set SRC_DIR=src

REM 创建构建目录
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%BUILD_DIR%\core" mkdir "%BUILD_DIR%\core"
if not exist "%BUILD_DIR%\lexer" mkdir "%BUILD_DIR%\lexer"
if not exist "%BUILD_DIR%\parser" mkdir "%BUILD_DIR%\parser"
if not exist "%BUILD_DIR%\vm" mkdir "%BUILD_DIR%\vm"
if not exist "%BUILD_DIR%\jit" mkdir "%BUILD_DIR%\jit"
if not exist "%BUILD_DIR%\api" mkdir "%BUILD_DIR%\api"
if not exist "%BUILD_DIR%\stdlib" mkdir "%BUILD_DIR%\stdlib"

echo 编译源文件...

REM 编译所有源文件
%CC% %CFLAGS% -I%INCLUDE_DIR% -c %SRC_DIR%\core\value.c -o %BUILD_DIR%\core\value.o
%CC% %CFLAGS% -I%INCLUDE_DIR% -c %SRC_DIR%\core\miniscript.c -o %BUILD_DIR%\core\miniscript.o
%CC% %CFLAGS% -I%INCLUDE_DIR% -c %SRC_DIR%\lexer\lexer.c -o %BUILD_DIR%\lexer\lexer.o
%CC% %CFLAGS% -I%INCLUDE_DIR% -c %SRC_DIR%\parser\parser.c -o %BUILD_DIR%\parser\parser.o
%CC% %CFLAGS% -I%INCLUDE_DIR% -c %SRC_DIR%\vm\vm.c -o %BUILD_DIR%\vm\vm.o
%CC% %CFLAGS% -I%INCLUDE_DIR% -c %SRC_DIR%\vm\chunk.c -o %BUILD_DIR%\vm\chunk.o
%CC% %CFLAGS% -I%INCLUDE_DIR% -c %SRC_DIR%\jit\jit.c -o %BUILD_DIR%\jit\jit.o

if errorlevel 1 (
    echo 编译失败！
    pause
    exit /b 1
)

echo 创建静态库...
ar rcs libminiscript.a %BUILD_DIR%\core\*.o %BUILD_DIR%\lexer\*.o %BUILD_DIR%\parser\*.o %BUILD_DIR%\vm\*.o %BUILD_DIR%\jit\*.o

echo 创建动态库...
%CC% -shared -o miniscript.dll %BUILD_DIR%\core\*.o %BUILD_DIR%\lexer\*.o %BUILD_DIR%\parser\*.o %BUILD_DIR%\vm\*.o %BUILD_DIR%\jit\*.o %LDFLAGS% -Wl,--out-implib,libminiscript.lib

echo 编译解释器...
%CC% %CFLAGS% -I%INCLUDE_DIR% -o miniscript.exe %SRC_DIR%\main.c -L. -lminiscript %LDFLAGS%

if errorlevel 1 (
    echo 链接失败！
    pause
    exit /b 1
)

echo.
echo === 编译成功！ ===
echo 生成的文件:
echo   - miniscript.exe     (解释器可执行文件)
echo   - libminiscript.a    (静态库)
echo   - miniscript.dll     (动态库)
echo   - libminiscript.lib  (导入库)
echo.
echo 运行测试:
echo   miniscript.exe examples\hello.ms
echo   test_runner.bat
echo.