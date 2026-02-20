# 测试 __init__ 和另一个方法

class Person:
    def __init__(self, name):
        self.name = name
    
    def greet(self):
        return "Hello"

p = Person("Alice")
print(p.name)
print(p.greet())
