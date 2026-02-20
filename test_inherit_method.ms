# 测试继承和方法

class Animal:
    def __init__(self, name):
        self.name = name

class Dog(Animal):
    def speak(self):
        return "Woof"

d = Dog("Buddy")
print(d.name)
print(d.speak())
