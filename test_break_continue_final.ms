"""
最终测试：break 和 continue 的所有边缘情况
"""

print("=== Break/Continue 最终测试 ===")
print()

# 测试 1: 四层嵌套
print("1. 四层嵌套循环")
for a in range(2):
    for b in range(2):
        for c in range(2):
            for d in range(2):
                if d == 1:
                    break
                print(f"  ({a},{b},{c},{d})")
print()

# 测试 2: 多个 break 在同一循环
print("2. 多个 break 条件")
for i in range(10):
    if i == 3:
        break
    if i == 5:
        break
    print(f"  i={i}")
print()

# 测试 3: 多个 continue 在同一循环
print("3. 多个 continue 条件")
for i in range(10):
    if i == 2:
        continue
    if i == 5:
        continue
    if i == 7:
        continue
    if i >= 8:
        break
    print(f"  i={i}")
print()

# 测试 4: break 和 continue 混合
print("4. break 和 continue 混合")
for i in range(10):
    if i % 2 == 0:
        continue
    if i > 7:
        break
    print(f"  i={i}")
print()

# 测试 5: while 嵌套 for
print("5. while 嵌套 for")
i = 0
while i < 3:
    for j in range(3):
        if j == 2:
            break
        print(f"  ({i},{j})")
    i = i + 1
print()

# 测试 6: for 嵌套 while
print("6. for 嵌套 while")
for i in range(3):
    j = 0
    while j < 3:
        if j == 2:
            break
        print(f"  ({i},{j})")
        j = j + 1
print()

print("=== 所有测试通过 ===")
