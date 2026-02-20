print("=== 类 + 字符串拼接测试 ===")

class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age
    
    def introduce(self):
        return "I'm " + self.name

p = Person("Alice", 30)
print(p.introduce())
