# 测试切片功能

print("=== List 切片测试 ===")
my_list = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
print("原始列表:", my_list)

# 基本切片
print("my_list[2:5]:", my_list[2:5])
print("my_list[:5]:", my_list[:5])
print("my_list[5:]:", my_list[5:])
print("my_list[:]:", my_list[:])

# 负索引
print("my_list[-3:]:", my_list[-3:])
print("my_list[:-3]:", my_list[:-3])
print("my_list[-5:-2]:", my_list[-5:-2])

# 步长
print("my_list[::2]:", my_list[::2])
print("my_list[1::2]:", my_list[1::2])
print("my_list[::3]:", my_list[::3])

# 反向切片
print("my_list[::-1]:", my_list[::-1])
print("my_list[::-2]:", my_list[::-2])
print("my_list[8:2:-1]:", my_list[8:2:-1])

print("\n=== String 切片测试 ===")
text = "Hello, World!"
print("原始字符串:", text)

print("text[0:5]:", text[0:5])
print("text[7:]:", text[7:])
print("text[:5]:", text[:5])
print("text[::2]:", text[::2])
print("text[::-1]:", text[::-1])

print("\n=== Tuple 切片测试 ===")
my_tuple = (10, 20, 30, 40, 50, 60)
print("原始元组:", my_tuple)

print("my_tuple[1:4]:", my_tuple[1:4])
print("my_tuple[::2]:", my_tuple[::2])
print("my_tuple[::-1]:", my_tuple[::-1])

print("\n=== 复杂切片测试 ===")
numbers = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

# 获取偶数
print("偶数:", numbers[::2])

# 获取奇数
print("奇数:", numbers[1::2])

# 反转
print("反转:", numbers[::-1])

# 每隔3个取一个
print("每隔3个:", numbers[::3])

# 从后往前每隔2个
print("从后往前每隔2个:", numbers[::-2])

print("\n=== 所有测试完成 ===")
