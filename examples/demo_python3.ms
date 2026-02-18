#!/usr/bin/env miniscript
# MiniScript Python 3 语法演示
# 展示已实现的 Python 3 核心功能

print("==================================================")
print("MiniScript - Python 3 语法演示")
print("==================================================")
print()

# 1. 基础数据类型和运算
print("【1. 基础数据类型和运算】")
a = 42
b = 3.14
c = "Hello, Python!"
d = True
e = None

print(f"整数: {a}")
print(f"浮点数: {b}")
print(f"字符串: {c}")
print(f"布尔值: {d}")
print(f"空值: {e}")
print()

# 2. 新增的运算符
print("【2. 算术运算符】")
print(f"加法: 10 + 5 = {10 + 5}")
print(f"减法: 10 - 5 = {10 - 5}")
print(f"乘法: 10 * 5 = {10 * 5}")
print(f"除法: 10 / 3 = {10 / 3}")
print(f"整除: 10 // 3 = {10 // 3}")
print(f"取模: 10 % 3 = {10 % 3}")
print(f"幂运算: 2 ** 10 = {2 ** 10}")
print()

# 3. 列表和切片
print("【3. 列表和切片】")
numbers = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
print(f"原列表: {numbers}")
print(f"切片 [2:5]: {numbers[2:5]}")
print(f"切片 [:5]: {numbers[:5]}")
print(f"切片 [5:]: {numbers[5:]}")
print(f"步长切片 [::2]: {numbers[::2]}")
print(f"反转 [::-1]: {numbers[::-1]}")
print()

# 4. 字典
print("【4. 字典】")
person = {"name": "Alice", "age": "30", "city": "Beijing"}
print(f"字典: {person}")
print(f"姓名: {person['name']}")
print(f"年龄: {person['age']}")
print()

# 5. 控制流
print("【5. 控制流】")
score = 85
if score >= 90:
    grade = "A"
elif score >= 80:
    grade = "B"
elif score >= 60:
    grade = "C"
else:
    grade = "D"
print(f"分数 {score} 对应等级 {grade}")
print()

# 6. for 循环
print("【6. for 循环】")
print("遍历列表:")
for num in [1, 2, 3, 4, 5]:
    print(f"  {num}")

print("遍历字符串:")
for ch in "Python":
    print(f"  {ch}")

print("使用 range:")
for i in range(5):
    print(f"  索引 {i}")
print()

# 7. while 循环
print("【7. while 循环】")
count = 0
while count < 5:
    print(f"  计数: {count}")
    count = count + 1
print()

# 8. 函数定义
print("【8. 函数定义】")

def add(x, y):
    # 两数相加
    return x + y

def multiply(x, y):
    # 两数相乘
    return x * y

print(f"add(10, 20) = {add(10, 20)}")
print(f"multiply(6, 7) = {multiply(6, 7)}")
print()

# 9. 递归函数
print("【9. 递归函数】")

def factorial(n):
    # 计算阶乘
    if n <= 1:
        return 1
    return n * factorial(n - 1)

def fibonacci(n):
    # 计算斐波那契数
    if n <= 1:
        return n
    return fibonacci(n - 1) + fibonacci(n - 2)

print(f"factorial(5) = {factorial(5)}")
print(f"factorial(10) = {factorial(10)}")
print("斐波那契数列前 10 项:")
for i in range(10):
    print(f"  fib({i}) = {fibonacci(i)}")
print()

# 10. 内置函数
print("【10. 内置函数】")

# 数学函数
nums = [3, 1, 4, 1, 5, 9, 2, 6]
print(f"列表: {nums}")
print(f"len() = {len(nums)}")
print(f"min() = {min(nums)}")
print(f"max() = {max(nums)}")
print(f"sum() = {sum(nums)}")
print(f"sorted() = {sorted(nums)}")
print(f"reversed() = {reversed(nums)}")

# 类型转换
print(f"int('123') = {int('123')}")
print(f"float('3.14') = {float('3.14')}")
print(f"str(42) = {str(42)}")
print(f"bool(1) = {bool(1)}")

# 字符串函数
print(f"chr(65) = {chr(65)}")
print(f"ord('A') = {ord('A')}")

# 类型检查
print(f"type(42) = {type(42)}")
print(f"isinstance(42, 'int') = {isinstance(42, 'int')}")
print()

# 11. 复杂表达式
print("【11. 复杂表达式】")
result1 = (2 ** 3 + 10) * 2 - 5 // 2
print(f"(2 ** 3 + 10) * 2 - 5 // 2 = {result1}")

result2 = 100 // 10 % 3 + 2 ** 3
print(f"100 // 10 % 3 + 2 ** 3 = {result2}")
print()

# 12. 嵌套结构
print("【12. 嵌套结构】")
matrix = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
print(f"矩阵: {matrix}")
print(f"第一行: {matrix[0]}")
print(f"元素 [1][2]: {matrix[1][2]}")

print("遍历矩阵:")
for row in matrix:
    for val in row:
        print(f"  {val}")
print()

# 13. 实用示例
print("【13. 实用示例】")

# 计算平方数
print("前 10 个平方数:")
squares = []
for i in range(1, 11):
    squares = squares + [i ** 2]
print(f"  {squares}")

# 过滤偶数
print("1-20 中的偶数:")
evens = []
for i in range(1, 21):
    if i % 2 == 0:
        evens = evens + [i]
print(f"  {evens}")

# 字符串处理
words = ["hello", "world", "python", "programming"]
print("单词长度:")
for word in words:
    print(f"  {word}: {len(word)}")
print()

print("==================================================")
print("演示完成！")
print("MiniScript 成功实现了 Python 3 的核心语法")
print("==================================================")
