# 测试 break 和 continue

print("=== break 和 continue 测试 ===")
print()

# 1. while 循环中的 break
print("1. while 循环中的 break")
i = 0
while i < 10:
    if i == 5:
        break
    print("  i =", i)
    i = i + 1
print("  循环结束，i =", i)
print()

# 2. while 循环中的 continue
print("2. while 循环中的 continue")
i = 0
while i < 5:
    i = i + 1
    if i == 3:
        continue
    print("  i =", i)
print("  循环结束")
print()

# 3. for 循环中的 break
print("3. for 循环中的 break")
for i in range(10):
    if i == 5:
        break
    print("  i =", i)
print("  循环结束")
print()

# 4. for 循环中的 continue
print("4. for 循环中的 continue")
for i in range(5):
    if i == 2:
        continue
    print("  i =", i)
print("  循环结束")
print()

# 5. 嵌套循环中的 break
print("5. 嵌套循环中的 break")
for i in range(3):
    print(f"  外层 i = {i}")
    for j in range(5):
        if j == 3:
            break
        print(f"    内层 j = {j}")
print("  循环结束")
print()

# 6. 嵌套循环中的 continue
print("6. 嵌套循环中的 continue")
for i in range(3):
    if i == 1:
        continue
    print(f"  外层 i = {i}")
    for j in range(3):
        if j == 1:
            continue
        print(f"    内层 j = {j}")
print("  循环结束")
print()

# 7. 混合使用 break 和 continue
print("7. 混合使用 break 和 continue")
for i in range(10):
    if i % 2 == 0:
        continue
    if i > 7:
        break
    print("  奇数:", i)
print("  循环结束")
print()

print("=== 所有测试完成 ===")
