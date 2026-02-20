print("=== 类方法测试 ===")

class Person:
    def __init__(self, name):
        self.name = name
    
    def greet(self):
        return "Hello"

p = Person("Alice")
print(p.name)
result = p.greet()
print(result)
