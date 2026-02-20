"""
测试 range 函数
"""

print("测试 range 函数")
print()

numbers = [1, 2, 3, 4, 5]
print(f"numbers = {numbers}")
print(f"len(numbers) = {len(numbers)}")
print()

print("测试 range(5):")
for i in range(5):
    print(f"  i = {i}")
print()

print("测试 range(len(numbers)):")
for i in range(len(numbers)):
    print(f"  i = {i}, numbers[i] = {numbers[i]}")
print()
