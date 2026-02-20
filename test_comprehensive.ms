"""
综合测试：列表推导式 + 三元表达式 + for 循环
"""

print("=== 综合测试 ===")
print()

# 测试1: 列表推导式后跟 for 循环
print("测试1: 列表推导式后跟 for 循环")
result = [x * 2 for x in [1, 2, 3]]
print(f"列表推导式结果: {result}")
for i in range(3):
    print(f"  for 循环: {i}")
print()

# 测试2: 三元表达式列表推导式后跟 for 循环
print("测试2: 三元表达式列表推导式后跟 for 循环")
numbers = [1, 2, 3, 4, 5]
parity = ["偶" if n % 2 == 0 else "奇" for n in numbers]
print(f"奇偶性: {parity}")
for p in parity:
    print(f"  {p}")
print()

# 测试3: 多个列表推导式
print("测试3: 多个列表推导式")
list1 = [x for x in range(5)]
list2 = [x * x for x in range(5)]
list3 = ["正" if x > 0 else "零或负" for x in [-1, 0, 1, 2]]
print(f"list1: {list1}")
print(f"list2: {list2}")
print(f"list3: {list3}")
print()

# 测试4: 嵌套 for 循环（不使用嵌套列表推导式）
print("测试4: 嵌套 for 循环")
for i in range(3):
    for j in range(3):
        print(f"  ({i}, {j})")
print()

# 测试5: 列表推导式在 for 循环内
print("测试5: 列表推导式在 for 循环内")
for i in range(3):
    squared = [x * x for x in range(i + 1)]
    print(f"  i={i}, 平方: {squared}")
print()

print("=== 所有测试通过 ===")
