# 测试当前已实现的功能

print("测试基础功能")
print("========================================")

# 1. 基础数据类型
a = 10
b = 3.14
c = "Hello"
d = True
e = None

print(f"整数: {a}")
print(f"浮点数: {b}")
print(f"字符串: {c}")
print(f"布尔值: {d}")
print(f"None: {e}")
print()

# 2. 运算符
print("运算符测试:")
print(f"加法: {a + 5}")
print(f"幂运算: {a ** 2}")
print(f"整除: {a // 3}")
print(f"取模: {a % 3}")
print()

# 3. 列表和切片
numbers = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
print(f"列表: {numbers}")
print(f"切片 [2:5]: {numbers[2:5]}")
print(f"反转: {numbers[::-1]}")
print()

# 4. 列表推导式
squares = [x**2 for x in range(5)]
print(f"列表推导式: {squares}")

# 注意：带条件的列表推导式需要使用三元表达式
evens = [x if x % 2 == 0 else 0 for x in range(10)]
print(f"带条件: {evens}")
print()

# 5. 控制流
print("控制流测试:")
for i in range(3):
    print(f"  循环 {i}")

score = 85
if score >= 90:
    grade = "A"
elif score >= 80:
    grade = "B"
else:
    grade = "C"
print(f"成绩: {grade}")
print()

# 6. 函数
def add(a, b):
    return a + b

def factorial(n):
    if n <= 1:
        return 1
    return n * factorial(n - 1)

print(f"add(3, 5) = {add(3, 5)}")
print(f"factorial(5) = {factorial(5)}")
print()

# 7. 内置函数
print("内置函数测试:")
print(f"len([1,2,3]) = {len([1, 2, 3])}")
print(f"sum([1,2,3,4,5]) = {sum([1, 2, 3, 4, 5])}")
print(f"max(3,1,4) = {max(3, 1, 4)}")
print(f"sorted([3,1,4,1,5]) = {sorted([3, 1, 4, 1, 5])}")
print()

print("========================================")
print("测试完成！")
