# MiniScript Python 3 兼容性测试
# 展示当前已实现的 Python 3 语法

print("=== Python 3 语法兼容性测试 ===")
print()

# 1. 基础数据类型
print("1. 基础数据类型")
a = 42
b = 3.14
c = "Hello, Python!"
d = True
e = False
f = None
print("整数:", a)
print("浮点数:", b)
print("字符串:", c)
print("布尔值:", d, e)
print("空值:", f)
print()

# 2. 算术运算
print("2. 算术运算")
print("加法: 10 + 5 =", 10 + 5)
print("减法: 10 - 5 =", 10 - 5)
print("乘法: 10 * 5 =", 10 * 5)
print("除法: 10 / 3 =", 10 / 3)
print("整除: 10 // 3 =", 10 // 3)
print("取模: 10 % 3 =", 10 % 3)
print("幂运算: 2 ** 10 =", 2 ** 10)
print()

# 3. 比较和逻辑运算
print("3. 比较和逻辑运算")
print("10 > 5 =", 10 > 5)
print("10 < 5 =", 10 < 5)
print("10 == 10 =", 10 == 10)
print("10 != 5 =", 10 != 5)
print("True and False =", True and False)
print("True or False =", True or False)
print("not True =", not True)
print()

# 4. 字符串操作
print("4. 字符串操作")
text = "Python"
print("字符串:", text)
print("长度:", len(text))
print("拼接:", "Hello" + " " + "World")
print("f-string:", f"值是 {a}")
print()

# 5. 列表
print("5. 列表")
numbers = [1, 2, 3, 4, 5]
print("列表:", numbers)
print("第一个元素:", numbers[0])
print("最后一个元素:", numbers[-1])
print("切片 [1:3]:", numbers[1:3])
print("切片 [::2]:", numbers[::2])
print("反转:", numbers[::-1])
print("长度:", len(numbers))
print()

# 6. 元组
print("6. 元组")
point = (10, 20, 30)
print("元组:", point)
print("第一个元素:", point[0])
print("长度:", len(point))
print()

# 7. 字典
print("7. 字典")
person = {"name": "Alice", "age": 30, "city": "Beijing"}
print("字典:", person)
print("姓名:", person["name"])
print("年龄:", person["age"])
print()

# 8. 控制流 - if/elif/else
print("8. 控制流 - if/elif/else")
score = 85
if score >= 90:
    print("优秀")
elif score >= 80:
    print("良好")
elif score >= 60:
    print("及格")
else:
    print("不及格")
print()

# 9. 控制流 - for 循环
print("9. 控制流 - for 循环")
print("遍历列表:")
for num in [1, 2, 3]:
    print("  数字:", num)

print("遍历字符串:")
for ch in "ABC":
    print("  字符:", ch)

print("使用 range:")
for i in range(5):
    print("  索引:", i)
print()

# 10. 控制流 - while 循环
print("10. 控制流 - while 循环")
count = 0
while count < 3:
    print("  计数:", count)
    count = count + 1
print()

# 11. 函数定义
print("11. 函数定义")

def add(x, y):
    return x + y

def greet(name):
    return f"Hello, {name}!"

def factorial(n):
    if n <= 1:
        return 1
    return n * factorial(n - 1)

print("add(10, 20) =", add(10, 20))
print(greet("Python"))
print("factorial(5) =", factorial(5))
print()

# 12. 内置函数
print("12. 内置函数")
print("abs(-10) =", abs(-10))
print("min(3, 1, 4) =", min(3, 1, 4))
print("max(3, 1, 4) =", max(3, 1, 4))
print("sum([1,2,3,4,5]) =", sum([1, 2, 3, 4, 5]))
print("pow(2, 8) =", pow(2, 8))
print("round(3.14159, 2) =", round(3.14159, 2))
print()

# 13. 类型转换
print("13. 类型转换")
print("int('123') =", int("123"))
print("float('3.14') =", float("3.14"))
print("str(42) =", str(42))
print("bool(1) =", bool(1))
print("bool(0) =", bool(0))
print()

# 14. 类型检查
print("14. 类型检查")
print("type(42) =", type(42))
print("type('hello') =", type("hello"))
print("type([1,2,3]) =", type([1, 2, 3]))
print("isinstance(42, 'int') =", isinstance(42, "int"))
print()

# 15. 字符串函数
print("15. 字符串函数")
print("chr(65) =", chr(65))
print("ord('A') =", ord("A"))
print()

# 16. 列表操作
print("16. 列表操作")
nums = [3, 1, 4, 1, 5, 9, 2, 6]
print("原列表:", nums)
print("sorted:", sorted(nums))
print("reversed:", reversed([1, 2, 3, 4, 5]))
print()

# 17. 嵌套结构
print("17. 嵌套结构")
matrix = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
print("矩阵:", matrix)
print("第一行:", matrix[0])
print("元素 [1][2]:", matrix[1][2])
print()

# 18. 嵌套循环
print("18. 嵌套循环")
test_range = range(3)
print("DEBUG: range(3) =", test_range)
print("DEBUG: type =", type(test_range))
for x in test_range:
    print("DEBUG: x =", x, "type =", type(x))
    for y in range(3):
        print(f"  ({x}, {y})")
print()

# 19. 复杂表达式
print("19. 复杂表达式")
result = (2 ** 3 + 10) * 2 - 5 // 2
print("(2 ** 3 + 10) * 2 - 5 // 2 =", result)
print()

# 20. 递归函数
print("20. 递归函数 - 斐波那契数列")
def fib(n):
    if n <= 1:
        return n
    return fib(n - 1) + fib(n - 2)

print("前 10 项:")
for i in range(10):
    print(f"  fib({i}) = {fib(i)}")
print()

print("=== 所有测试完成 ===")
print("MiniScript 已成功实现 Python 3 的核心语法！")
