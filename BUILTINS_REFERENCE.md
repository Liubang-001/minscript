# MiniScript 内置函数参考

本文档列出了 MiniScript 中所有可用的 Python 风格内置函数。

## 输入输出函数

### print(*args)
打印一个或多个值到标准输出。

```python
print("Hello, World!")
print("x =", 42)
print([1, 2, 3])  # 输出: [1, 2, 3]
```

### input(prompt="")
从标准输入读取一行文本。

```python
name = input("Enter your name: ")
print("Hello,", name)
```

## 类型转换函数

### int(x)
将值转换为整数。

```python
int("123")      # 123
int(3.14)       # 3
int(True)       # 1
```

### float(x)
将值转换为浮点数。

```python
float("3.14")   # 3.14
float(42)       # 42.0
float(True)     # 1.0
```

### str(x)
将值转换为字符串。

```python
str(42)         # "42"
str(3.14)       # "3.14"
str(True)       # "True"
str(None)       # "None"
```

### bool(x)
将值转换为布尔值。

```python
bool(1)         # True
bool(0)         # False
bool("")        # False
bool("hello")   # True
```

## 集合函数

### len(obj)
返回对象的长度。支持字符串、列表、元组和字典。

```python
len("hello")        # 5
len([1, 2, 3])      # 3
len((1, 2))         # 2
len({"a": 1})       # 1
```

### range(start, stop, step)
生成一个整数序列的列表。

```python
range(5)            # [0, 1, 2, 3, 4]
range(2, 5)         # [2, 3, 4]
range(0, 10, 2)     # [0, 2, 4, 6, 8]
range(5, 0, -1)     # [5, 4, 3, 2, 1]
```

### list(iterable)
将可迭代对象转换为列表。

```python
list((1, 2, 3))     # [1, 2, 3]
list([1, 2, 3])     # [1, 2, 3] (复制)
```

### tuple(iterable)
将可迭代对象转换为元组。

```python
tuple([1, 2, 3])    # (1, 2, 3)
tuple((1, 2, 3))    # (1, 2, 3) (返回原值)
```

### dict()
创建一个新的空字典。

```python
d = dict()          # {}
```

## 数学函数

### abs(x)
返回数字的绝对值。

```python
abs(-5)             # 5
abs(3.14)           # 3.14
abs(-2.5)           # 2.5
```

### min(*args)
返回参数中的最小值。

```python
min(3, 1, 4, 1, 5)  # 1
min(2.5, 1.5, 3.5)  # 1.5
```

### max(*args)
返回参数中的最大值。

```python
max(3, 1, 4, 1, 5)  # 5
max(2.5, 1.5, 3.5)  # 3.5
```

### sum(iterable)
返回可迭代对象中所有元素的和。

```python
sum([1, 2, 3, 4, 5])    # 15
sum([1.5, 2.5, 3.0])    # 7.0
```

### pow(base, exp)
返回 base 的 exp 次幂。

```python
pow(2, 3)           # 8.0
pow(10, 2)          # 100.0
pow(2, -1)          # 0.5
```

### round(number, ndigits=0)
将数字四舍五入到指定的小数位数。

```python
round(3.14159)      # 3
round(3.14159, 2)   # 3.14
round(3.14159, 4)   # 3.1416
```

## 字符串函数

### chr(i)
返回 Unicode 码点为整数 i 的字符。

```python
chr(65)             # "A"
chr(97)             # "a"
chr(48)             # "0"
```

### ord(c)
返回字符的 Unicode 码点。

```python
ord("A")            # 65
ord("a")            # 97
ord("0")            # 48
```

## 工具函数

### type(obj)
返回对象的类型名称。

```python
type(42)            # "int"
type(3.14)          # "float"
type("hello")       # "str"
type([1, 2])        # "list"
type((1, 2))        # "tuple"
type({"a": 1})      # "dict"
type(True)          # "bool"
type(None)          # "NoneType"
```

### isinstance(obj, type_name)
检查对象是否为指定类型。

```python
isinstance(42, "int")           # True
isinstance("hello", "str")      # True
isinstance([1, 2], "list")      # True
isinstance(42, "str")           # False
```

### enumerate(iterable, start=0)
返回一个包含索引和值的元组列表。

```python
for index, value in enumerate(["a", "b", "c"]):
    print(index, value)
# 输出:
# 0 a
# 1 b
# 2 c

for index, value in enumerate(["a", "b", "c"], 1):
    print(index, value)
# 输出:
# 1 a
# 2 b
# 3 c
```

### zip(*iterables)
将多个可迭代对象打包成元组列表。

```python
for item in zip([1, 2, 3], ["a", "b", "c"]):
    print(item)
# 输出:
# (1, a)
# (2, b)
# (3, c)

numbers = [1, 2, 3]
letters = ["a", "b", "c"]
for num, letter in zip(numbers, letters):
    print(num, letter)
```

### sorted(iterable)
返回排序后的新列表。

```python
sorted([3, 1, 4, 1, 5])     # [1, 1, 3, 4, 5]
sorted([2.5, 1.5, 3.5])     # [1.5, 2.5, 3.5]
```

### reversed(iterable)
返回反转后的新列表。

```python
reversed([1, 2, 3])         # [3, 2, 1]
reversed([5, 4, 3, 2, 1])   # [1, 2, 3, 4, 5]
```

## 使用示例

### 综合示例

```python
# 类型转换
age = int(input("Enter your age: "))
print("You are", age, "years old")

# 集合操作
numbers = range(1, 11)
print("Numbers:", numbers)
print("Sum:", sum(numbers))
print("Min:", min(*numbers))
print("Max:", max(*numbers))

# 字符串处理
text = "Hello"
for i, char in enumerate(text):
    print(i, char, ord(char))

# 列表操作
data = [3, 1, 4, 1, 5, 9, 2, 6]
print("Original:", data)
print("Sorted:", sorted(data))
print("Reversed:", reversed(data))

# 多列表处理
names = ["Alice", "Bob", "Charlie"]
ages = [25, 30, 35]
for name, age in zip(names, ages):
    print(name, "is", age, "years old")
```

## 注意事项

1. 所有函数都遵循 Python 3 的语法和行为
2. 某些函数（如 `sorted`）使用简单的冒泡排序，对大数据集可能较慢
3. `print` 函数会自动格式化列表、元组和字典的输出
4. 字符串中的 `\n` 等转义字符目前不被解析（待实现）

## 扩展性

可以通过扩展系统添加更多内置函数。参见 `src/builtins/builtins.c` 和 `src/builtins/builtins.h`。
