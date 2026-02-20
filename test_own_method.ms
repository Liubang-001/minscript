# 测试自己的方法（不继承）

class Dog:
    def __init__(self, name):
        self.name = name
    
    def speak(self):
        return "Woof"

d = Dog("Buddy")
print(d.name)
print(d.speak())
