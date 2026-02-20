"""
列表推导式 + 三元表达式测试
实现 Python 语法: result_list = ["偶数" if n % 2 == 0 else "奇数" for n in numbers]
"""

print("=== 列表推导式 + 三元表达式测试 ===")
print()

# 测试1: 基本的三元表达式列表推导式
print("测试1: 判断奇偶数")
numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
result_list = ["偶数" if n % 2 == 0 else "奇数" for n in numbers]
print(f"数字: {numbers}")
print(f"分类: {result_list}")
print()

# 测试2: 数值计算
print("测试2: 数值计算")
values = [1, 2, 3, 4, 5]
doubled_or_tripled = [n * 2 if n % 2 == 0 else n * 3 for n in values]
print(f"原始值: {values}")
print(f"偶数x2, 奇数x3: {doubled_or_tripled}")
print()

# 测试3: 字符串处理
print("测试3: 字符串处理")
scores = [85, 92, 58, 73, 45, 96]
grades = ["及格" if s >= 60 else "不及格" for s in scores]
print(f"分数: {scores}")
print(f"等级: {grades}")
print()

# 测试4: 嵌套三元表达式
print("测试4: 嵌套三元表达式")
ages = [5, 15, 25, 65, 75]
categories = ["儿童" if age < 12 else ("青少年" if age < 18 else ("成人" if age < 60 else "老年")) for age in ages]
print(f"年龄: {ages}")
print(f"分类: {categories}")
print()

# 测试5: 简单映射（无三元表达式）
print("测试5: 简单映射")
nums = [1, 2, 3, 4, 5]
squared = [n * n for n in nums]
print(f"原始: {nums}")
print(f"平方: {squared}")
print()

# 测试6: 使用 range
print("测试6: 使用 range")
evens = ["b" if i % 2 == 0 else "a" for i in range(10)]
print(f"0-9 的奇偶: {evens}")
print(evens)
print()

print("=== 所有测试完成 ===")
