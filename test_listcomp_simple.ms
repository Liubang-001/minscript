"""
简单列表推导式测试
"""

print("=== 简单列表推导式测试 ===")
print()

# 测试1: 简单的变量映射
numbers = [1, 2, 3, 4, 5]
result1 = [x for x in numbers]
print(f"原始列表: {numbers}")
print(f"简单映射: {result1}")
print()

# 测试2: 使用range
result2 = [x for x in range(5)]
print(f"range(5): {result2}")
print()

print("=== 测试完成 ===")
