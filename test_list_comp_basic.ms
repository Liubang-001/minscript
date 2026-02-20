# 测试基本列表推导式

print("=== 列表推导式测试 ===")
print()

# 测试 1: 简单列表推导式
print("测试 1: 简单列表推导式")
simple = [x for x in range(5)]
print(simple)
print()

# 测试 2: 乘法
print("测试 2: 乘法")
mult = [x * 2 for x in range(5)]
print(mult)
print()

# 测试 3: 幂运算
print("测试 3: 幂运算")
squares = [x ** 2 for x in range(5)]
print(squares)
print()

print("=== 测试完成 ===")
