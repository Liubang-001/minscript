# MiniScript 当前支持的 Python 3 语法示例
# 这个文件展示了 MiniScript 已经实现的功能

print("=== MiniScript 支持的 Python 3 语法 ===")
print()

# ==================== 1. 基础语法 ====================
print("1. 基础语法")
a = 10
b = 3.14
c = "Hello"
d = True
e = None

print("变量:", a, b, c, d, e)

# 运算符
sum_val = a + 5
power = a ** 2
floor_div = a // 3
remainder = a % 3
is_equal = (a == 10)

print("运算结果:", sum_val, power, floor_div, remainder, is_equal)

# f-string 格式化
print(f"a = {a}, b = {b}, c = {c}")
print()

# ==================== 2. 数据结构 ====================
print("2. 数据结构")

# 列表
fruits = ["apple", "banana", "cherry"]
print("列表:", fruits)
print("第一个元素:", fruits[0])
print("列表长度:", len(fruits))

# 元组
point = (10, 20)
print("元组:", point)

# 字典
person = {"name": "Alice", "age": 30}
print("字典:", person)
print("姓名:", person["name"])

# 切片
numbers = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
print("原列表:", numbers)
print("切片 [2:5]:", numbers[2:5])
print("切片 [:5]:", numbers[:5])
print("切片 [5:]:", numbers[5:])
print("切片 [::2]:", numbers[::2])
print("反转 [::-1]:", numbers[::-1])
print()

# ==================== 3. 控制流 ====================
print("3. 控制流")

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

# enumerate
print("enumerate:")
for idx in enumerate(["a", "b", "c"]):
    print(f"  {idx}")
print()

# ==================== 4. 函数 ====================
print("4. 函数")

def add(a, b):
    return a + b

def greet(name):
    return f"Hello, {name}"

print("add(3, 5) =", add(3, 5))
print(greet("Alice"))

# 递归函数
def factorial(n):
    if n <= 1:
        return 1
    return n * factorial(n - 1)

print("factorial(5) =", factorial(5))
print()

# ==================== 5. 内置函数 ====================
print("5. 内置函数")

# 类型转换
print("int('123') =", int("123"))
print("float('3.14') =", float("3.14"))
print("str(42) =", str(42))
print("bool(1) =", bool(1))

# 集合函数
print("len([1,2,3]) =", len([1, 2, 3]))
print("range(5) =", range(5))
print("list((1,2,3)) =", list((1, 2, 3)))
print("tuple([1,2,3]) =", tuple([1, 2, 3]))

# 数学函数
print("abs(-5) =", abs(-5))
print("min(3,1,4) =", min(3, 1, 4))
print("max(3,1,4) =", max(3, 1, 4))
print("sum([1,2,3,4,5]) =", sum([1, 2, 3, 4, 5]))
print("pow(2, 3) =", pow(2, 3))
print("round(3.14159, 2) =", round(3.14159, 2))

# 字符串函数
print("chr(65) =", chr(65))
print("ord('A') =", ord("A"))

# 工具函数
print("type(42) =", type(42))
print("isinstance(42, 'int') =", isinstance(42, "int"))

# sorted 和 reversed
print("sorted([3,1,4,1,5]) =", sorted([3, 1, 4, 1, 5]))
print("reversed([1,2,3]) =", reversed([1, 2, 3]))
print()

# ==================== 6. 字符串操作 ====================
print("6. 字符串操作")
text = "Python"
print("原字符串:", text)
print("遍历字符:")
for ch in text:
    print(f"  {ch}")
print()

# ==================== 7. 嵌套结构 ====================
print("7. 嵌套结构")

# 嵌套列表
matrix = [[1, 2], [3, 4]]
print("矩阵:", matrix)
print("第一行:", matrix[0])
print("元素 [0][1]:", matrix[0][1])

# 嵌套循环
print("嵌套循环:")
for i in range(2):
    for j in range(2):
        print(f"  ({i}, {j})")
print()

# ==================== 8. 复杂示例 ====================
print("8. 复杂示例")

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
print("单词长度:", lengths)
print()

print("=== 所有测试完成 ===")
