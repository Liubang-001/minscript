"""
测试 for range(len(...))
"""

print("测试 for range(len(...))")
print()

numbers = [1, 2, 3]
print(f"numbers = {numbers}")
print(f"len(numbers) = {len(numbers)}")
print()

print("测试1: for i in range(3):")
for i in range(3):
    print(f"  i = {i}")
print()

print("测试2: for i in range(len(numbers)):")
n = len(numbers)
print(f"  n = {n}")
for i in range(n):
    print(f"  i = {i}")
print()

print("测试3: 直接 for i in range(len(numbers)):")
for i in range(len(numbers)):
    print(f"  i = {i}")
print()
