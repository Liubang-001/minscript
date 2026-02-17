# MiniScript - Python 3 语法支持

MiniScript现在支持Python 3风格的语法！

## 主要特性

### 1. print() 函数调用（而非语句）
```python
# Python 3 风格
print("Hello, World!")
print("Result:", 42)
print(1 + 2, "equals", 3)

# 不再支持Python 2风格
# print "Hello"  # 错误！
```

### 2. True/False/None 关键字
```python
# Python 3 布尔值和空值
print(True)   # 输出: True
print(False)  # 输出: False
print(None)   # 输出: None

# 也支持小写（兼容性）
print(true)   # 输出: True
print(false)  # 输出: False
print(nil)    # 输出: None
```

### 3. 完整的表达式支持
```python
# 算术运算
print(1 + 2 * 3)  # 7
print((1 + 2) * 3)  # 9

# 字符串连接
print("Hello" + " " + "World")

# 比较运算
print(5 > 3)  # True
print(2 == 2)  # True
print(1 != 2)  # True

# 逻辑运算
print(not True)  # False
```

## 当前支持的语法

### 数据类型
- 整数: `42`, `-17`
- 浮点数: `3.14`, `-2.5`
- 字符串: `"Hello"`, `"世界"`
- 布尔值: `True`, `False`
- 空值: `None`

### 运算符
- 算术: `+`, `-`, `*`, `/`
- 比较: `==`, `!=`, `>`, `>=`, `<`, `<=`
- 逻辑: `not` (一元)
- 字符串连接: `+`

### 内置函数
- `print(*args)` - 打印多个参数，空格分隔
- `len(string)` - 返回字符串长度

## 示例代码

### 基础示例
```python
print("=== MiniScript Demo ===")

# 变量和计算
print("计算:", 10 + 5 * 2)

# 字符串操作
print("Hello" + " " + "Python 3!")

# 布尔逻辑
print("5 > 3:", 5 > 3)
print("not False:", not False)

# 多参数print
print("答案是:", 42)
```

### 运行示例
```bash
# Windows
miniscript.exe examples\py3_full.ms

# Linux/Mac
./miniscript examples/py3_full.ms
```

## 与Python 3的区别

### 已实现
✅ `print()` 函数调用  
✅ `True`/`False`/`None` 关键字  
✅ 基本算术和逻辑运算  
✅ 字符串操作  

### 未实现（计划中）
❌ 变量赋值 (`x = 10`)  
❌ 函数定义 (`def func():`)  
❌ 控制流 (`if`, `while`, `for`)  
❌ 列表和字典  
❌ 类和对象  
❌ 模块导入  
❌ 异常处理  

## 注意事项

1. 注释使用 `#`，但目前在某些情况下可能导致解析错误
2. 每个语句需要在单独的行上
3. 不支持续行符 `\`
4. 字符串只支持双引号 `"`，不支持单引号 `'`

## 下一步开发

1. 修复注释处理
2. 添加变量赋值支持
3. 实现 `if`/`else` 控制流
4. 添加 `while` 和 `for` 循环
5. 支持函数定义
6. 实现列表和字典

## 贡献

欢迎提交PR来扩展Python 3语法支持！