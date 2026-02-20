# 测试继承

class Animal:
    def __init__(self, name):
        self.name = name

class Dog(Animal):
    def dummy(self):
        pass

d = Dog("Buddy")
print(d.name)
