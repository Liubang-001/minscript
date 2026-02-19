"""
MiniScript 循环控制演示
展示 break 和 continue 的实际应用
"""

print("==================================================")
print("MiniScript 循环控制演示")
print("==================================================")
print()

# 示例 1: 查找质数
print("示例 1: 查找 2-20 之间的质数")
for num in range(2, 21):
    is_prime = True
    for i in range(2, num):
        if num % i == 0:
            is_prime = False
            break
    if is_prime:
        print(f"  {num} 是质数")
print()

# 示例 2: 跳过特定值
print("示例 2: 打印 1-10，跳过 3 的倍数")
for i in range(1, 11):
    if i % 3 == 0:
        continue
    print(f"  {i}")
print()

# 示例 3: 提前退出
print("示例 3: 计算累加和，超过 50 时停止")
total = 0
for i in range(1, 100):
    total = total + i
    if total > 50:
        print(f"  在 i={i} 时，总和={total}，超过 50")
        break
print()

# 示例 4: 嵌套循环 - 打印乘法表（部分）
print("示例 4: 打印 1-5 的乘法表（结果小于 20）")
for i in range(1, 6):
    for j in range(1, 6):
        result = i * j
        if result >= 20:
            break
        print(f"  {i} x {j} = {result}")
print()

# 示例 5: 数据验证
print("示例 5: 验证列表中的数据")
numbers = [1, 2, 3, -1, 5, 6]
print(f"  检查列表: {numbers}")
all_positive = True
for num in numbers:
    if num < 0:
        print(f"  发现负数: {num}")
        all_positive = False
        break
if all_positive:
    print("  所有数字都是正数")
print()

# 示例 6: 跳过错误数据
print("示例 6: 处理数据，跳过无效值")
data = [10, 0, 20, 0, 30, 40]
print(f"  原始数据: {data}")
print("  处理结果:")
for value in data:
    if value == 0:
        print("    跳过无效值 0")
        continue
    result = 100 // value
    print(f"    100 / {value} = {result}")
print()

print("==================================================")
print("演示完成")
print("==================================================")
