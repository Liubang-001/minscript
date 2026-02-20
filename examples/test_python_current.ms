#!/usr/bin/env miniscript
# -*- coding: utf-8 -*-

"""
Python 3 语法特性测试 - 当前已实现的功能
基于 examples/test.py，只包含当前 MiniScript 已支持的语法
"""

print("=" * 60)
print("Python 3 语法特性测试 - 当前已实现的功能")
print("=" * 60)
print()

# ==================== 1. 基础语法 ====================
print("====== 1. 基础语法 ======")

# 变量赋值
a = 10
b = 3.14
c = "Hello"
d = True
e = None

print(f"变量: a={a}, b={b}, c={c}, d={d}, e={e}")

# 运算符
sum_ab = a + b
power = a ** 2
floor_div = a // 3
remainder = a % 3
is_equal = (a == 10)

print(f"运算: sum={sum_ab}, power={power}, floor_div={floor_div}, mod={remainder}, equal={is_equal}")

# f-string 格式化
print(f"f-string: a = {a}, b = {b:.2f}, c = '{c}'")

# 注意：海象运算符 := 尚未实现
# if (n := len(c)) > 4:
#     print(f"字符串长度 {n} > 4")

print()

# ==================== 2. 数据结构 ====================
print("====== 2. 数据结构 ======")

# 列表
fruits = ["apple", "banana", "cherry"]
print(f"列表: {fruits}")
print(f"第一个元素: {fruits[0]}")
print(f"列表长度: {len(fruits)}")

# 元组
point = (10, 20)
print(f"元组: {point}")

# 字典
person = {"name": "Alice", "age": 30, "city": "New York"}
print(f"字典: {person}")
print(f"姓名: {person['name']}")

# 集合 - 注意：集合类型尚未实现
# numbers_set = {1, 2, 3, 3, 4}
# print(f"集合: {numbers_set}")

# 推导式 - 列表推导式已实现
squares = [x**2 for x in range(5)]
print(f"列表推导式: {squares}")

# 带条件的列表推导式
even_squares = [x**2 for x in range(10) if x % 2 == 0]
print(f"条件推导式: {even_squares}")

# 带三元表达式的列表推导式
numbers = [1, 2, 3, 4, 5]
result_list = ["偶数" if n % 2 == 0 else "奇数" for n in numbers]
print(f"三元推导式: {result_list}")

# 注意：集合推导式和字典推导式尚未实现
# even_squares_set = {x**2 for x in range(5) if x % 2 == 0}
# square_dict = {x: x**2 for x in range(5)}

# 切片
numbers = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
print(f"原列表: {numbers}")
print(f"切片 [2:5]: {numbers[2:5]}")
print(f"切片 [:5]: {numbers[:5]}")
print(f"切片 [5:]: {numbers[5:]}")
print(f"切片 [::2]: {numbers[::2]}")
print(f"反转 [::-1]: {numbers[::-1]}")

print()

# ==================== 3. 控制流 ====================
print("====== 3. 控制流 ======")

# if-elif-else
score = 85
if score >= 90:
    grade = "A"
elif score >= 80:
    grade = "B"
else:
    grade = "C"
print(f"分数 {score} 对应等级 {grade}")

# for 循环
print("for 循环:")
for i in range(3):
    print(f"  第 {i} 次")

# while 循环
print("while 循环:")
count = 0
while count < 3:
    print(f"  计数 {count}")
    count = count + 1

# break / continue
print("break/continue:")
for i in range(5):
    if i == 2:
        continue
    if i == 4:
        break
    print(f"  i = {i}")

# 注意：for/while 的 else 子句尚未实现
# for i in range(3):
#     print(i)
# else:
#     print("循环正常结束")

# enumerate
print("enumerate:")
for item in enumerate(["apple", "banana"]):
    print(f"  {item}")

print()

# ==================== 4. 函数 ====================
print("====== 4. 函数 ======")

# 普通函数
def add(a, b):
    return a + b

def greet(name):
    return f"Hello, {name}"

print(f"add(3, 5) = {add(3, 5)}")
print(f"greet('Alice') = {greet('Alice')}")

# 注意：默认参数尚未实现
# def greet_with_default(name, greeting="Hello"):
#     return f"{greeting}, {name}"

# 注意：*args 和 **kwargs 尚未实现
# def sum_all(*args):
#     return sum(args)

# 注意：关键字参数尚未实现
# def safe_divide(a, b, *, precision=2):
#     return round(a / b, precision)

# 注意：lambda 表达式尚未实现
# multiply = lambda x, y: x * y
# print(f"lambda: {multiply(4, 5)}")

# 递归函数
def factorial(n):
    if n <= 1:
        return 1
    return n * factorial(n - 1)

print(f"factorial(5) = {factorial(5)}")

print()

# ==================== 5. 内置函数 ====================
print("====== 5. 内置函数 ======")

# 类型转换
print(f"int('123') = {int('123')}")
print(f"float('3.14') = {float('3.14')}")
print(f"str(42) = {str(42)}")
print(f"bool(1) = {bool(1)}")

# 集合函数
print(f"len([1,2,3]) = {len([1, 2, 3])}")
print(f"list((1,2,3)) = {list((1, 2, 3))}")
print(f"tuple([1,2,3]) = {tuple([1, 2, 3])}")

# 数学函数
print(f"abs(-5) = {abs(-5)}")
print(f"min(3,1,4) = {min(3, 1, 4)}")
print(f"max(3,1,4) = {max(3, 1, 4)}")
print(f"sum([1,2,3,4,5]) = {sum([1, 2, 3, 4, 5])}")
print(f"pow(2, 3) = {pow(2, 3)}")
print(f"round(3.14159, 2) = {round(3.14159, 2)}")

# 字符串函数
print(f"chr(65) = {chr(65)}")
print(f"ord('A') = {ord('A')}")

# 工具函数
print(f"type(42) = {type(42)}")
print(f"isinstance(42, 'int') = {isinstance(42, 'int')}")
print(f"sorted([3,1,4,1,5]) = {sorted([3, 1, 4, 1, 5])}")
print(f"reversed([1,2,3]) = {reversed([1, 2, 3])}")

print()

# ==================== 6. 字符串操作 ====================
print("====== 6. 字符串操作 ======")

text = "Python"
print(f"原字符串: {text}")
print("遍历字符:")
for ch in text:
    print(f"  {ch}")

print()

# ==================== 7. 嵌套结构 ====================
print("====== 7. 嵌套结构 ======")

# 嵌套列表
matrix = [[1, 2], [3, 4]]
print(f"矩阵: {matrix}")
print(f"第一行: {matrix[0]}")
print(f"元素 [0][1]: {matrix[0][1]}")

# 嵌套循环
print("嵌套循环:")
for i in range(2):
    for j in range(2):
        print(f"  ({i}, {j})")

print()

# ==================== 8. 复杂示例 ====================
print("====== 8. 复杂示例 ======")

# 斐波那契数列
def fibonacci(n):
    if n <= 1:
        return n
    return fibonacci(n - 1) + fibonacci(n - 2)

print("斐波那契数列前 10 项:")
fib_list = []
for i in range(10):
    fib_list = fib_list + [fibonacci(i)]
print(fib_list)

# 字符串处理
words = ["hello", "world", "python"]
lengths = []
for w in words:
    lengths = lengths + [len(w)]
print(f"单词长度: {lengths}")

print()

# ==================== 未实现功能说明 ====================
print("=" * 60)
print("未实现的功能（参考 examples/test.py）：")
print("=" * 60)
print()
print("1. 类和面向对象")
print("   - class 定义")
print("   - __init__, __str__, __repr__ 等魔术方法")
print("   - 继承和 super()")
print("   - @property, @classmethod, @staticmethod")
print()
print("2. 异常处理")
print("   - try/except/else/finally")
print("   - raise 语句")
print("   - 自定义异常类")
print()
print("3. 高级函数特性")
print("   - lambda 表达式")
print("   - 默认参数")
print("   - *args 和 **kwargs")
print("   - 关键字参数")
print()
print("4. 上下文管理器")
print("   - with 语句")
print("   - __enter__ 和 __exit__")
print()
print("5. 装饰器")
print("   - @decorator 语法")
print("   - 带参数的装饰器")
print("   - 类装饰器")
print()
print("6. 生成器和迭代器")
print("   - yield 语句")
print("   - __iter__ 和 __next__")
print("   - 生成器表达式")
print()
print("7. 异步编程")
print("   - async/await")
print("   - 异步上下文管理器")
print("   - 异步生成器")
print()
print("8. 其他语法")
print("   - 海象运算符 :=")
print("   - assert 语句")
print("   - del 语句")
print("   - for/while else 子句")
print("   - global/nonlocal")
print("   - eval/exec")
print()
print("=" * 60)
print("当前测试完成！")
print("=" * 60)
