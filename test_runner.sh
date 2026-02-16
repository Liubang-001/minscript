#!/bin/bash

# MiniScript 测试运行脚本

echo "=== MiniScript 测试套件 ==="
echo

# 检查解释器是否存在
if [ ! -f "./miniscript" ]; then
    echo "错误: 找不到 miniscript 解释器"
    echo "请先运行 'make' 编译项目"
    exit 1
fi

echo "1. 运行基础语法测试..."
echo "----------------------------------------"
./miniscript examples/syntax_test.ms
echo
echo "----------------------------------------"
echo

echo "2. 运行综合功能测试..."
echo "----------------------------------------"
./miniscript examples/comprehensive_test.ms
echo
echo "----------------------------------------"
echo

echo "3. 运行性能测试..."
echo "----------------------------------------"
./miniscript examples/performance_test.ms
echo
echo "----------------------------------------"
echo

echo "4. 运行Hello World示例..."
echo "----------------------------------------"
./miniscript examples/hello.ms
echo
echo "----------------------------------------"
echo

echo "5. 运行斐波那契示例..."
echo "----------------------------------------"
./miniscript examples/fibonacci.ms
echo
echo "----------------------------------------"
echo

echo "6. 测试REPL模式（输入 'exit' 退出）..."
echo "----------------------------------------"
echo "输入一些表达式来测试REPL:"
echo "例如: print 1 + 2"
echo "例如: print \"Hello REPL\""
echo "输入 Ctrl+C 退出REPL"
echo
./miniscript

echo
echo "=== 测试完成 ==="