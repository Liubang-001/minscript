# MiniScript Python特性简单演示

print("=== MiniScript Python特性演示 ===\n")

# 1. Lambda表达式
print("1. Lambda表达式:")
add = lambda x, y: x + y
square = lambda x: x * x
print("  add(3, 5) =", add(3, 5))
print("  square(4) =", square(4))
result = (lambda x: x * 2)(5)
print("  (lambda x: x * 2)(5) =", result)

# 2. 列表推导式
print("\n2. 列表推导式:")
numbers = [1, 2, 3, 4, 5]
squares = [x * x for x in numbers]
print("  原列表:", numbers)
print("  平方:", squares)

# 3. 默认参数
print("\n3. 默认参数:")
def greet(name, greeting="Hello"):
    return greeting + ", " + name + "!"

print("  greet('Alice') =", greet("Alice"))
print("  greet('Bob', 'Hi') =", greet("Bob", "Hi"))

# 4. 类和继承
print("\n4. 类和继承:")
class Animal:
    def __init__(self, name):
        self.name = name
    
    def speak(self):
        return "Some sound"

class Dog(Animal):
    def speak(self):
        return "Woof!"

dog = Dog("Buddy")
print("  dog.name =", dog.name)
print("  dog.speak() =", dog.speak())

# 5. 魔术方法
print("\n5. 魔术方法:")
class Vector:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    
    def __add__(self, other):
        return Vector(self.x + other.x, self.y + other.y)
    
    def __sub__(self, other):
        return Vector(self.x - other.x, self.y - other.y)
    
    def __mul__(self, scalar):
        return Vector(self.x * scalar, self.y * scalar)
    
    def __eq__(self, other):
        return self.x == other.x and self.y == other.y
    
    def __lt__(self, other):
        return (self.x * self.x + self.y * self.y) < (other.x * other.x + other.y * other.y)
    
    def __getitem__(self, index):
        if index == 0:
            return self.x
        return self.y

v1 = Vector(3, 4)
v2 = Vector(1, 2)
v3 = v1 + v2
v4 = v1 - v2
v5 = v1 * 2

print("  v1 = Vector(3, 4)")
print("  v2 = Vector(1, 2)")
print("  v1 + v2 = Vector(", v3.x, ",", v3.y, ")")
print("  v1 - v2 = Vector(", v4.x, ",", v4.y, ")")
print("  v1 * 2 = Vector(", v5.x, ",", v5.y, ")")
print("  v1 == v2 =", v1 == v2)
print("  v1 < v2 =", v1 < v2)
print("  v1 > v2 =", v1 > v2)
print("  v1[0] =", v1[0], ", v1[1] =", v1[1])

# 6. 内置函数
print("\n6. 内置函数:")
nums = [1, 2, 3, 4, 5]
print("  len([1,2,3,4,5]) =", len(nums))
print("  sum([1,2,3,4,5]) =", sum(nums))
print("  min([1,2,3,4,5]) =", min(nums))
print("  max([1,2,3,4,5]) =", max(nums))
print("  abs(-10) =", abs(-10))
print("  pow(2, 3) =", pow(2, 3))

print("\n=== 演示完成！ ===")
