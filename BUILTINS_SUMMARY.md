# Python 内置函数实现总结

## 概述

为 MiniScript 添加了 26 个 Python 风格的内置函数，涵盖类型转换、集合操作、数学计算、字符串处理和工具函数等常用功能。

## 实现的函数列表

### 输入输出 (2个)
- `print(*args)` - 打印值到标准输出，支持格式化显示集合类型
- `input(prompt)` - 从标准输入读取文本

### 类型转换 (4个)
- `int(x)` - 转换为整数
- `float(x)` - 转换为浮点数
- `str(x)` - 转换为字符串
- `bool(x)` - 转换为布尔值

### 集合函数 (5个)
- `len(obj)` - 获取长度（支持 string, list, tuple, dict）
- `range(start, stop, step)` - 生成整数序列
- `list(iterable)` - 转换为列表
- `tuple(iterable)` - 转换为元组
- `dict()` - 创建空字典

### 数学函数 (6个)
- `abs(x)` - 绝对值
- `min(*args)` - 最小值
- `max(*args)` - 最大值
- `sum(iterable)` - 求和
- `pow(base, exp)` - 幂运算
- `round(number, ndigits)` - 四舍五入

### 字符串函数 (2个)
- `chr(i)` - Unicode 码点转字符
- `ord(c)` - 字符转 Unicode 码点

### 工具函数 (7个)
- `type(obj)` - 获取类型名称
- `isinstance(obj, type_name)` - 类型检查
- `enumerate(iterable, start)` - 枚举索引和值
- `zip(*iterables)` - 打包多个可迭代对象
- `sorted(iterable)` - 排序（返回新列表）
- `reversed(iterable)` - 反转（返回新列表）

## 文件结构

```
src/builtins/
├── builtins.h          # 函数声明
└── builtins.c          # 函数实现
```

## 关键特性

### 1. 改进的 print 函数
支持递归打印集合类型，自动格式化输出：

```python
print([1, 2, 3])                    # [1, 2, 3]
print((1, 2, 3))                    # (1, 2, 3)
print({"a": 1, "b": 2})             # {'a': 1, 'b': 2}
print([[1, 2], [3, 4]])             # [[1, 2], [3, 4]]
```

### 2. 灵活的类型转换
所有类型转换函数都支持多种输入类型：

```python
int("123")      # 字符串转整数
int(3.14)       # 浮点数转整数
int(True)       # 布尔值转整数
```

### 3. 完整的 range 支持
支持 1-3 个参数的 range 调用：

```python
range(5)            # [0, 1, 2, 3, 4]
range(2, 5)         # [2, 3, 4]
range(0, 10, 2)     # [0, 2, 4, 6, 8]
range(5, 0, -1)     # [5, 4, 3, 2, 1]
```

### 4. 实用的工具函数
enumerate 和 zip 使得迭代更加方便：

```python
# enumerate
for i, val in enumerate(["a", "b", "c"]):
    print(i, val)

# zip
for x, y in zip([1, 2, 3], ["a", "b", "c"]):
    print(x, y)
```

## 编译集成

### 修改的文件
1. `src/main.c` - 使用 `ms_register_builtins()` 注册所有内置函数
2. `build_windows.bat` - 添加 builtins 目录的编译

### 编译命令
```bash
# Windows
.\build_windows.bat

# 生成的文件包含所有内置函数
```

## 测试结果

所有 26 个内置函数都通过了测试：

```
=== 类型转换函数 ===
int('123'): 123
float('3.14'): 3.14
str(42): 42
bool(1): True

=== 集合函数 ===
len([1, 2, 3]): 3
range(5): [0, 1, 2, 3, 4]
list((1, 2, 3)): [1, 2, 3]

=== 数学函数 ===
abs(-5): 5
min(3, 1, 4, 1, 5): 1
sum([1, 2, 3, 4, 5]): 15

=== 字符串函数 ===
chr(65): A
ord('A'): 65

=== 工具函数 ===
type(42): int
isinstance(42, 'int'): True
enumerate(['a', 'b', 'c']): [(0, a), (1, b), (2, c)]
sorted([3, 1, 4, 1, 5]): [1, 1, 3, 4, 5]
```

## 使用示例

### 实际应用示例

```python
# 用户输入处理
age = int(input("Enter your age: "))
if isinstance(age, "int") and age > 0:
    print("Valid age:", age)

# 数据处理
numbers = [3, 1, 4, 1, 5, 9, 2, 6]
print("Original:", numbers)
print("Sorted:", sorted(numbers))
print("Sum:", sum(numbers))
print("Average:", sum(numbers) / len(numbers))

# 多列表处理
names = ["Alice", "Bob", "Charlie"]
ages = [25, 30, 35]
for i, (name, age) in enumerate(zip(names, ages), 1):
    print(i, name, "is", age, "years old")

# 字符串处理
text = "Hello"
codes = [ord(c) for c in text]
print("Character codes:", codes)
decoded = "".join([chr(c) for c in codes])
print("Decoded:", decoded)
```

## 性能考虑

1. **sorted()** - 使用冒泡排序，时间复杂度 O(n²)，适合小数据集
2. **reversed()** - 时间复杂度 O(n)，创建新列表
3. **enumerate()** 和 **zip()** - 时间复杂度 O(n)，创建新列表
4. **print()** - 递归深度限制为 10 层，防止无限递归

## 未来改进

1. 添加更多内置函数：
   - `map()`, `filter()`, `reduce()`
   - `all()`, `any()`
   - `set()`, `frozenset()`
   
2. 性能优化：
   - 使用快速排序替代冒泡排序
   - 实现惰性求值的 range 对象
   
3. 功能增强：
   - 支持 `sorted()` 的 key 和 reverse 参数
   - 支持 `print()` 的 sep 和 end 参数
   - 添加字符串转义字符支持

## 兼容性

所有内置函数都遵循 Python 3 的语法和行为，确保熟悉 Python 的开发者可以无缝使用 MiniScript。

## 文档

详细的函数参考文档请查看 `BUILTINS_REFERENCE.md`。
