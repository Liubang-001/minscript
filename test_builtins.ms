# 测试 Python 内置函数

print("=== 类型转换函数 ===")
print("int('123'):", int("123"))
print("float('3.14'):", float("3.14"))
print("str(42):", str(42))
print("bool(1):", bool(1))
print("bool(0):", bool(0))

print("\n=== 集合函数 ===")
print("len([1, 2, 3]):", len([1, 2, 3]))
print("len('hello'):", len("hello"))
print("range(5):", range(5))
print("range(2, 5):", range(2, 5))
print("list((1, 2, 3)):", list((1, 2, 3)))
print("tuple([1, 2, 3]):", tuple([1, 2, 3]))

print("\n=== 数学函数 ===")
print("abs(-5):", abs(-5))
print("min(3, 1, 4, 1, 5):", min(3, 1, 4, 1, 5))
print("max(3, 1, 4, 1, 5):", max(3, 1, 4, 1, 5))
print("sum([1, 2, 3, 4, 5]):", sum([1, 2, 3, 4, 5]))
print("pow(2, 3):", pow(2, 3))
print("round(3.14159, 2):", round(3.14159, 2))

print("\n=== 字符串函数 ===")
print("chr(65):", chr(65))
print("ord('A'):", ord("A"))

print("\n=== 工具函数 ===")
print("type(42):", type(42))
print("type('hello'):", type("hello"))
print("type([1, 2]):", type([1, 2]))
print("isinstance(42, 'int'):", isinstance(42, "int"))
print("isinstance('hello', 'str'):", isinstance("hello", "str"))

print("\n=== enumerate 函数 ===")
for item in enumerate(["a", "b", "c"]):
    print(item)

print("\n=== zip 函数 ===")
for item in zip([1, 2, 3], ["a", "b", "c"]):
    print(item)

print("\n=== sorted 函数 ===")
print("sorted([3, 1, 4, 1, 5]):", sorted([3, 1, 4, 1, 5]))

print("\n=== reversed 函数 ===")
print("reversed([1, 2, 3]):", reversed([1, 2, 3]))

print("\n=== 所有测试完成 ===")
