"""
循环控制测试
测试 break 和 continue 的所有功能
"""

print("=== 循环控制完整测试 ===")
print()

# 1. 基本 break
print("1. 基本 break 测试")
count = 0
for i in range(10):
    if i == 5:
        break
    count = count + 1
print(f"  循环执行了 {count} 次")
print()

# 2. 基本 continue
print("2. 基本 continue 测试")
count = 0
for i in range(10):
    if i % 2 == 0:
        continue
    count = count + 1
print(f"  处理了 {count} 个奇数")
print()

# 3. while 循环中的 break
print("3. while 循环中的 break")
i = 0
while True:
    if i >= 5:
        break
    print(f"  i = {i}")
    i = i + 1
print()

# 4. while 循环中的 continue
print("4. while 循环中的 continue")
i = 0
while i < 5:
    i = i + 1
    if i == 3:
        continue
    print(f"  i = {i}")
print()

# 5. 嵌套循环 - 内层 break
print("5. 嵌套循环 - 内层 break")
for i in range(3):
    for j in range(5):
        if j == 2:
            break
        print(f"  ({i}, {j})")
print()

# 6. 嵌套循环 - 外层 continue
print("6. 嵌套循环 - 外层 continue")
for i in range(4):
    if i == 2:
        continue
    for j in range(2):
        print(f"  ({i}, {j})")
print()

# 7. 实际应用：查找元素
print("7. 实际应用：查找元素")
numbers = [10, 20, 30, 40, 50]
target = 30
found = False
index = 0

for i in range(len(numbers)):
    if numbers[i] == target:
        found = True
        index = i
        break

if found:
    print(f"  找到 {target}，位置: {index}")
else:
    print(f"  未找到 {target}")
print()

# 8. 实际应用：过滤数据
print("8. 实际应用：过滤数据")
print("  1-20 中能被 3 整除但不能被 2 整除的数:")
for i in range(1, 21):
    if i % 2 == 0:
        continue
    if i % 3 != 0:
        continue
    print(f"    {i}")
print()

# 9. 混合使用
print("9. 混合使用 break 和 continue")
print("  处理 1-20，跳过偶数，遇到 15 停止:")
for i in range(1, 21):
    if i % 2 == 0:
        continue
    if i >= 15:
        break
    print(f"    {i}")
print()

# 10. 复杂嵌套
print("10. 复杂嵌套循环")
for i in range(3):
    if i == 1:
        continue
    for j in range(3):
        if j == 1:
            continue
        for k in range(3):
            if k == 2:
                break
            print(f"    ({i}, {j}, {k})")
print()

print("=== 所有测试完成 ===")
