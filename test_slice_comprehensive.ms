# 全面的切片功能测试

print("=== Python 风格切片功能测试 ===\n")

# 1. 列表切片
print("1. 列表切片")
numbers = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

for i in numbers[2:5]:
    print(i)

for x in range(1, 10):
    print(x)

print("原始列表:", numbers)
print("  [2:5]    =", numbers[2:5])
print("  [:5]     =", numbers[:5])
print("  [5:]     =", numbers[5:])
print("  [:]      =", numbers[:])
print("  [-3:]    =", numbers[-3:])
print("  [:-3]    =", numbers[:-3])
print("  [::2]    =", numbers[::2])
print("  [::-1]   =", numbers[::-1])
print()

# 2. 字符串切片
print("2. 字符串切片")
text = "Python Programming"
print("原始字符串:", text)
print("  [0:6]    =", text[0:6])
print("  [7:]     =", text[7:])
print("  [:6]     =", text[:6])
print("  [::2]    =", text[::2])
print("  [::-1]   =", text[::-1])
print()

# 3. 元组切片
print("3. 元组切片")
colors = ("red", "green", "blue", "yellow", "purple")
print("原始元组:", colors)
print("  [1:3]    =", colors[1:3])
print("  [::2]    =", colors[::2])
print("  [::-1]   =", colors[::-1])
print()

# 4. 实际应用示例
print("4. 实际应用示例")

# 获取偶数和奇数
nums = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
print("偶数索引:", nums[::2])
print("奇数索引:", nums[1::2])

# 反转序列
word = "hello"
print("反转单词:", word[::-1])

# 获取最后N个元素
data = [10, 20, 30, 40, 50, 60, 70, 80, 90, 100]
print("最后3个:", data[-3:])

# 跳过首尾
items = [1, 2, 3, 4, 5, 6, 7, 8]
print("跳过首尾:", items[1:-1])

# 每隔N个取一个
sequence = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
print("每隔3个:", sequence[::3])
print()

# 5. 复杂切片
print("5. 复杂切片操作")
matrix = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
print("原始:", matrix)
print("中间部分 [2:-2]:", matrix[2:-2])
print("反向每隔2个 [::-2]:", matrix[::-2])
print("从5到1反向 [5:1:-1]:", matrix[5:1:-1])
print()

# 6. 边界情况
print("6. 边界情况测试")
small = [1, 2, 3]
print("小列表:", small)
print("  [10:]    =", small[10:])
print("  [:10]    =", small[:10])
print("  [-10:]   =", small[-10:])
print()

# 7. 字符串处理示例
print("7. 字符串处理")
sentence = "The quick brown fox"
print("原句:", sentence)
print("前3个单词:", sentence[:14])
print("反转:", sentence[::-1])
print("每隔一个字符:", sentence[::2])
print()

print("=== 所有切片测试完成 ===")
