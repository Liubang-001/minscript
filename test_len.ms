"""
测试 len 函数
"""

print("测试 len 函数")
print()

numbers = [1, 2, 3, 4, 5]
print(f"numbers = {numbers}")
print(f"len(numbers) = {len(numbers)}")
print()

result = ["偶数" if n % 2 == 0 else "奇数" for n in numbers]
print(f"result = {result}")
print(f"len(result) = {len(result)}")
print()

print("手动打印 result 的元素:")
print(f"result[0] = {result[0]}")
print(f"result[1] = {result[1]}")
print(f"result[2] = {result[2]}")
print(f"result[3] = {result[3]}")
print(f"result[4] = {result[4]}")
