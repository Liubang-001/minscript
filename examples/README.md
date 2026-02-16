# MiniScript 示例文件说明

本目录包含了MiniScript解释器的各种测试和示例文件。

## 文件说明

### 基础示例
- **hello.ms** - Hello World和基本语法演示
- **fibonacci.ms** - 斐波那契数列示例（简化版）

### 测试文件
- **syntax_test.ms** - 完整的语法特性测试
- **comprehensive_test.ms** - 综合功能测试
- **performance_test.ms** - 性能和JIT测试
- **error_test.ms** - 错误处理测试

## 当前支持的语法特性

### 数据类型
- **布尔值**: `true`, `false`
- **空值**: `nil`
- **整数**: `42`, `-17`, `0`
- **浮点数**: `3.14`, `-2.5`, `0.0`
- **字符串**: `"Hello"`, `"包含中文"`

### 运算符
- **算术运算**: `+`, `-`, `*`, `/`
- **比较运算**: `==`, `!=`, `>`, `>=`, `<`, `<=`
- **逻辑运算**: `!` (逻辑非)
- **一元运算**: `-` (负号)

### 表达式
- **括号表达式**: `(1 + 2) * 3`
- **运算符优先级**: `2 + 3 * 4` = `14`
- **字符串连接**: `"Hello" + " " + "World"`

### 语句
- **print语句**: `print "Hello"`
- **表达式语句**: `1 + 2`

## 运行示例

### 编译项目
```bash
make
```

### 运行单个示例
```bash
# Linux/Mac
./miniscript examples/hello.ms

# Windows
miniscript.exe examples\hello.ms
```

### 运行所有测试
```bash
# Linux/Mac
./test_runner.sh

# Windows
test_runner.bat
```

### REPL模式
```bash
# Linux/Mac
./miniscript

# Windows
miniscript.exe
```

## 测试输出示例

运行 `syntax_test.ms` 应该产生类似以下的输出：

```
=== 基本数据类型 ===
true
false
nil
42
-17
0
3.14
-2.5
0
Hello
World

包含中文的字符串
=== 算术运算 ===
3
5
5
7
3.3
-7
20
10
-6
5
2
-4
14
9
8
10
...
```

## 注意事项

1. 当前实现是基础版本，不支持变量声明、函数定义、控制流等高级特性
2. JIT编译器是简化实现，主要用于演示概念
3. 错误处理还比较基础，某些错误可能导致程序崩溃
4. 字符串内存管理需要进一步优化

## 扩展建议

要扩展更多语法特性，可以考虑添加：
- 变量声明和赋值 (`var x = 10`)
- 函数定义 (`func add(a, b): return a + b`)
- 控制流 (`if`, `while`, `for`)
- 数据结构 (列表、字典)
- 模块系统 (`import`, `from`)