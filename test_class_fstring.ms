print("=== 类 + f-string 测试 ===")

class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age
    
    def introduce(self):
        return f"I'm {self.name}, {self.age} years old"

p = Person("Alice", 30)
print(p.introduce())
