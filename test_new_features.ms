#!/usr/bin/env miniscript
# 测试新实现的 Python 3 语法特性

print("=" * 50)
print("测试新实现的 Python 3 语法特性")
print("=" * 50)
print()

# ==================== 1. lambda 表达式 ====================
print("1. lambda 表达式")
print("-" * 50)

# 简单 lambda
add = lambda x, y: x + y
print(f"add(3, 5) = {add(3, 5)}")

multiply = lambda x, y: x * y
print(f"multiply(4, 6) = {multiply(4, 6)}")

# 单参数 lambda
square = lambda x: x ** 2
print(f"square(7) = {square(7)}")

# lambda 在列表中使用
operations = [
    lambda x: x + 1,
    lambda x: x * 2,
    lambda x: x ** 2
]
print("对数字 5 应用操作:")
for i in range(len(operations)):
    result = operations[i](5)
    print(f"  操作 {i}: {result}")

print()

# ==================== 2. assert 语句 ====================
print("2. assert 语句")
print("-" * 50)

x = 10
assert x == 10
print("assert x == 10 通过")

assert x > 5
print("assert x > 5 通过")

# 带消息的 assert
assert x < 100
print("assert x < 100 通过")

print()

# ==================== 3. del 语句 ====================
print("3. del 语句")
print("-" * 50)

# 删除变量
temp = 42
print(f"temp = {temp}")
del temp
print("del temp 执行成功")

# 删除列表元素
numbers = [1, 2, 3, 4, 5]
print(f"原列表: {numbers}")
del numbers[2]
print(f"del numbers[2] 后: {numbers}")

# 删除字典键
person = {"name": "Alice", "age": 30, "city": "NYC"}
print(f"原字典: {person}")
del person["age"]
print(f"del person['age'] 后: {person}")

print()

# ==================== 4. for/while else 子句 ====================
print("4. for/while else 子句")
print("-" * 50)

# for else - 正常结束
print("for else (正常结束):")
for i in range(3):
    print(f"  循环 {i}")
else:
    print("  for 循环正常结束")

# for else - break 中断
print("\nfor else (break 中断):")
for i in range(5):
    print(f"  循环 {i}")
    if i == 2:
        print("  遇到 break")
        break
else:
    print("  for 循环正常结束")

# while else - 正常结束
print("\nwhile else (正常结束):")
count = 0
while count < 3:
    print(f"  计数 {count}")
    count = count + 1
else:
    print("  while 循环正常结束")

# while else - break 中断
print("\nwhile else (break 中断):")
count = 0
while count < 10:
    print(f"  计数 {count}")
    if count == 2:
        print("  遇到 break")
        break
    count = count + 1
else:
    print("  while 循环正常结束")

print()

# ==================== 5. 默认参数 ====================
print("5. 默认参数")
print("-" * 50)

def greet(name, greeting="Hello"):
    return f"{greeting}, {name}!"

print(greet("Alice"))
print(greet("Bob", "Hi"))
print(greet("Charlie", "Good morning"))

def power(base, exponent=2):
    return base ** exponent

print(f"power(5) = {power(5)}")
print(f"power(5, 3) = {power(5, 3)}")

print()

# ==================== 6. 综合示例 ====================
print("6. 综合示例")
print("-" * 50)

# 使用 lambda 和列表操作
numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
print(f"原数字列表: {numbers}")

# 过滤偶数
is_even = lambda x: x % 2 == 0
evens = []
for n in numbers:
    if is_even(n):
        evens = evens + [n]
print(f"偶数: {evens}")

# 查找第一个大于 5 的数
print("\n查找第一个大于 5 的数:")
for n in numbers:
    if n > 5:
        print(f"  找到: {n}")
        break
else:
    print("  未找到")

print()
print("=" * 50)
print("所有测试完成！")
print("=" * 50)
