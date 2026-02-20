"""
最终演示：Python 风格的列表推导式 + 三元表达式
"""

print("=== Python 语法演示 ===")
print()

# 原始需求：判断奇偶数
numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
result_list = ["偶数" if n % 2 == 0 else "奇数" for n in numbers]

print(f"数字: {numbers}")
print(f"分类: {result_list}")
print()

# 验证：列表推导式后可以正常使用 for 循环
print("验证 for 循环正常工作:")
for i in range(5):
    print(f"  {i}: {result_list[i]}")
print()

# 更多示例
scores = [85, 92, 58, 73, 45, 96]
grades = ["及格" if s >= 60 else "不及格" for s in scores]
print(f"分数: {scores}")
print(f"等级: {grades}")
print()

# 数值计算
values = [1, 2, 3, 4, 5]
doubled_or_tripled = [n * 2 if n % 2 == 0 else n * 3 for n in values]
print(f"原始值: {values}")
print(f"偶数x2, 奇数x3: {doubled_or_tripled}")
print()

print("=== 所有功能正常 ===")
