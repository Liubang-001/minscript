# 功能总结测试

print("=== 1. 基本类定义 ===")
class Animal:
    def __init__(self, name):
        self.name = name

a = Animal("Dog")
print(a.name)

print("")
print("=== 2. 继承 ===")
class Dog(Animal):
    def speak(self):
        return "Woof"

d = Dog("Buddy")
print(d.name)
print(d.speak())

print("")
print("=== 3. Lambda ===")
square = lambda x: x * x
print(square(5))

print("")
print("=== 4. 默认参数 ===")
def greet(name, greeting="Hello"):
    return greeting + " " + name

print(greet("Alice"))
print(greet("Bob", "Hi"))

print("")
print("=== 5. Assert ===")
assert True, "This should pass"
print("Assert passed")

print("")
print("=== 6. 列表推导式 ===")
numbers = [1, 2, 3, 4, 5]
squares = [x * x for x in numbers]
print(squares)

print("")
print("=== 所有测试完成 ===")
