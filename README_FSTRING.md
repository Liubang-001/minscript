# MiniScript F-String 功能

## 概述

MiniScript 现已支持 Python 3 风格的 f-string（格式化字符串字面量），允许在字符串中直接嵌入表达式。

## 快速开始

### 基本用法

```miniscript
name = "Alice"
age = 25
msg = f"Hello {name}, you are {age} years old"
print(msg)
# 输出: Hello Alice, you are 25 years old
```

### 表达式

```miniscript
x = 10
y = 20
result = f"The sum of {x} and {y} is {x + y}"
print(result)
# 输出: The sum of 10 and 20 is 30
```

## 语法

### 基本格式

```
f"text {expression} more text"
f'text {expression} more text'
```

### 支持的表达式

- 变量: `f"{name}"`
- 算术: `f"{x + y}"`
- 函数调用: `f"{len(text)}"`
- 比较: `f"{x > y}"`
- 逻辑: `f"{x and y}"`
- 复杂表达式: `f"{(a + b) * c}"`

## 示例

### 示例 1: 变量插值

```miniscript
first_name = "John"
last_name = "Doe"
full_name = f"{first_name} {last_name}"
print(full_name)  # 输出: John Doe
```

### 示例 2: 算术表达式

```miniscript
width = 10
height = 5
area = f"Area: {width * height}"
print(area)  # 输出: Area: 50
```

### 示例 3: 多个表达式

```miniscript
name = "Alice"
age = 25
city = "New York"
info = f"{name} is {age} years old and lives in {city}"
print(info)
# 输出: Alice is 25 years old and lives in New York
```

### 示例 4: 布尔值

```miniscript
is_active = True
status = f"Active: {is_active}"
print(status)  # 输出: Active: True
```

### 示例 5: 复杂表达式

```miniscript
a = 7
b = 3
c = 2
result = f"({a} + {b}) * {c} = {(a + b) * c}"
print(result)  # 输出: (7 + 3) * 2 = 20
```

## 类型转换

F-string 中的表达式结果自动转换为字符串：

| 类型 | 转换结果 |
|------|---------|
| int | 十进制数字 |
| float | 浮点数字 |
| bool | "True" 或 "False" |
| nil | "None" |
| string | 保持不变 |

### 转换示例

```miniscript
# 整数
x = 42
msg = f"The answer is {x}"  # "The answer is 42"

# 浮点数
pi = 3.14
msg = f"Pi is {pi}"  # "Pi is 3.14"

# 布尔值
flag = True
msg = f"Flag: {flag}"  # "Flag: True"

# nil
value = nil
msg = f"Value: {value}"  # "Value: None"
```

## 工作原理

### 编译时转换

F-string 在编译时转换为字符串连接操作：

```miniscript
# 原始 f-string
msg = f"Hello {name}, you are {age} years old"

# 编译为等价代码
msg = "Hello " + name + ", you are " + age + " years old"
```

### 运行时执行

1. 计算每个表达式
2. 将结果转换为字符串
3. 使用字符串连接操作符 (+) 连接所有部分

## 性能

- **编译时转换**: F-string 在编译时转换为字符串连接
- **无运行时开销**: 性能与手动字符串连接相同
- **内存效率**: 内存使用与等价的字符串连接相同

## 限制

- ❌ 不支持格式说明符（如 `{x:.2f}`）
- ❌ 不支持表达式中的嵌套 f-string
- ❌ 表达式中不能包含字符串字面量

## 测试

### 运行测试

```bash
# 基本测试
miniscript.exe test_fstring.ms

# 完整示例
miniscript.exe examples/fstring_example.ms
```

### 测试覆盖

✅ 简单 f-string
✅ 多个表达式
✅ 算术表达式
✅ 布尔值
✅ 类型转换
✅ 复杂表达式
✅ 嵌套算术

## 与 Python 的对比

### 相同点

- 使用 `f"..."` 语法
- 支持任意表达式
- 自动类型转换

### 不同点

| 特性 | MiniScript | Python |
|------|-----------|--------|
| 格式说明符 | ❌ | ✅ |
| 嵌套 f-string | ❌ | ✅ |
| 字符串字面量 | ❌ | ✅ |

## 常见问题

### Q: 如何在 f-string 中使用字符串？

A: 目前不支持。使用字符串连接代替：

```miniscript
# 不支持
msg = f"Hello {'world'}"

# 使用字符串连接
msg = "Hello " + "world"
```

### Q: 如何格式化数字？

A: 目前不支持格式说明符。使用字符串连接代替：

```miniscript
# 不支持
msg = f"Pi is {3.14159:.2f}"

# 使用字符串连接
msg = "Pi is 3.14"
```

### Q: 性能如何？

A: F-string 在编译时转换为字符串连接，性能与手动字符串连接相同。

## 实现细节

### 词法分析

- 检测 `f"..."` 或 `f'...'` 模式
- 返回 `TOKEN_FSTRING` token

### 语法分析

- 提取 f-string 内容
- 查找 `{...}` 表达式
- 为每个表达式创建新的解析器
- 生成字节码

### 虚拟机

- 增强 `OP_ADD` 处理器
- 支持任意类型与字符串的连接
- 自动类型转换

## 文件修改

- `src/lexer/lexer.h` - 添加 TOKEN_FSTRING
- `src/lexer/lexer.c` - 实现 f-string 检测
- `src/parser/parser.c` - 实现 f-string 解析
- `src/vm/vm.c` - 增强 OP_ADD 处理器

## 向后兼容性

✅ 完全向后兼容：
- 现有的字符串处理继续工作
- 现有的脚本无需修改
- 所有现有测试通过

## 总结

MiniScript 的 f-string 功能提供了一种简洁、高效的字符串格式化方式，与 Python 3 的 f-string 语法兼容。该功能已完全实现并测试通过，可用于生产环境。
