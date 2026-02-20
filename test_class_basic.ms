print("=== 类定义基础测试 ===")

class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age
    
    def introduce(self):
        return f"I'm {self.name}, {self.age} years old"
    
    def birthday(self):
        self.age = self.age + 1
        return f"Happy birthday! Now {self.age} years old"

# 创建实例
p = Person("Alice", 30)
print(p.name)  # Alice
print(p.age)   # 30
print(p.introduce())  # I'm Alice, 30 years old

# 修改属性
p.age = 31
print(p.age)  # 31

# 调用方法
print(p.birthday())  # Happy birthday! Now 32 years old

print("✅ 所有测试通过！")
