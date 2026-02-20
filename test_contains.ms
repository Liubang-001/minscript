# 测试 in 运算符和 __contains__ 魔术方法

# 测试基本的 in 运算符
print("=== Basic in operator ===")
nums = [1, 2, 3]
print("2 in [1, 2, 3]:", 2 in nums)
print("4 in [1, 2, 3]:", 4 in nums)

letters = ("a", "b", "c")
print("'a' in ('a', 'b', 'c'):", "a" in letters)
print("'d' in ('a', 'b', 'c'):", "d" in letters)

# 测试字符串
print("\n=== String contains ===")
print("'hello' in 'hello world':", "hello" in "hello world")
print("'xyz' in 'hello world':", "xyz" in "hello world")

# 测试字典
print("\n=== Dict contains ===")
d = {"a": 1, "b": 2}
print("'a' in dict:", "a" in d)
print("'c' in dict:", "c" in d)

# 测试 __contains__ 魔术方法
print("\n=== __contains__ magic method ===")
class MyContainer:
    def __init__(self):
        self.items = [1, 2, 3, 4, 5]
    
    def __contains__(self, item):
        print(f"__contains__ called with {item}")
        return item in self.items

container = MyContainer()
print("3 in container:", 3 in container)
print("6 in container:", 6 in container)
