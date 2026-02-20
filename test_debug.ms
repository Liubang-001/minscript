"""
调试测试
"""

print("测试1: 简单变量")
x = 5
print(f"x = {x}")
print()

print("测试2: 简单列表")
numbers = [1, 2, 3]
print(f"numbers = {numbers}")
print()

print("测试3: 简单for循环")
for n in numbers:
    print(f"n = {n}")
print()

print("测试4: 尝试列表推导式")
result = [x for x in numbers]
print(f"result = {result}")

