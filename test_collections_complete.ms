# 完整的集合类型测试

# 1. List 测试
print("=== List 测试 ===")
my_list = [1, 2, 3, 4, 5]
print("List:")
for x in my_list:
    print(x)

# 2. Tuple 测试
print("\n=== Tuple 测试 ===")
my_tuple = (10, 20, 30)
print("Tuple:")
for y in my_tuple:
    print(y)

# 3. Dict 测试
print("\n=== Dict 测试 ===")
my_dict = {"name": "Alice", "age": 30, "city": "Beijing"}
print("Dict keys:")
for key in my_dict:
    print(key)

# 4. String 测试
print("\n=== String 测试 ===")
my_string = "Python"
print("String:")
for char in my_string:
    print(char)

# 5. 嵌套循环测试
print("\n=== 嵌套循环测试 ===")
for i in [1, 2]:
    for j in [10, 20]:
        print(i + j)

print("\n=== 所有测试完成 ===")
