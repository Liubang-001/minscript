# 测试继承功能

class Animal:
    def __init__(self, name):
        self.name = name
    
    def speak(self):
        return "Some sound"

class Dog(Animal):
    def speak(self):
        return "Woof!"

# 测试基类
animal = Animal("Generic")
print(animal.name)
print(animal.speak())

# 测试子类
dog = Dog("Buddy")
print(dog.name)
print(dog.speak())

