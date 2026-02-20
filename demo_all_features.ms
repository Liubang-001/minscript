# MiniScript Python特性演示
# 展示所有已实现的功能

print("=" * 50)
print("MiniScript Python特性演示")
print("=" * 50)

# 1. Lambda表达式
print("\n1. Lambda表达式:")
add = lambda x, y: x + y
square = lambda x: x * x
print(f"  add(3, 5) = {add(3, 5)}")
print(f"  square(4) = {square(4)}")
print(f"  (lambda x: x * 2)(5) = {(lambda x: x * 2)(5)}")

# 2. 列表推导式
print("\n2. 列表推导式:")
numbers = [1, 2, 3, 4, 5]
squares = [x * x for x in numbers]
evens = [x for x in numbers if x % 2 == 0]
print("  squares =", squares)
print("  evens =", evens)

# 3. 默认参数
print("\n3. 默认参数:")
def greet(name, greeting="Hello"):
    return f"{greeting}, {name}!"

print(f"  greet('Alice') = {greet('Alice')}")
print(f"  greet('Bob', 'Hi') = {greet('Bob', 'Hi')}")

# 4. for/while else
print("\n4. for/while else:")
for i in range(3):
    print(f"  Loop {i}")
else:
    print("  Loop completed normally")

# 5. assert语句
print("\n5. assert语句:")
x = 10
assert x > 5, "x should be greater than 5"
print("  assert x > 5 passed")

# 6. 类和继承
print("\n6. 类和继承:")
class Animal:
    def __init__(self, name):
        self.name = name
    
    def speak(self):
        return "Some sound"

class Dog(Animal):
    def speak(self):
        return "Woof!"

dog = Dog("Buddy")
print(f"  dog.name = {dog.name}")
print(f"  dog.speak() = {dog.speak()}")

# 7. 魔术方法
print("\n7. 魔术方法:")
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
    
    def __str__(self):
        return f"Vector({self.x}, {self.y})"
    
    def __getitem__(self, index):
        if index == 0:
            return self.x
        else:
            return self.y

v1 = Vector(3, 4)
v2 = Vector(1, 2)
v3 = v1 + v2
print(f"  v1 = Vector(3, 4)")
print(f"  v2 = Vector(1, 2)")
print(f"  v1 + v2 = Vector({v3.x}, {v3.y})")
print(f"  v1 - v2 = Vector({(v1 - v2).x}, {(v1 - v2).y})")
print(f"  v1 * 2 = Vector({(v1 * 2).x}, {(v1 * 2).y})")
print(f"  v1 == v2 = {v1 == v2}")
print(f"  v1 < v2 = {v1 < v2}")
print(f"  v1 > v2 = {v1 > v2}")
print(f"  v1[0] = {v1[0]}, v1[1] = {v1[1]}")

# 8. 内置函数
print("\n8. 内置函数:")
nums = [1, 2, 3, 4, 5]
print(f"  len([1,2,3,4,5]) = {len(nums)}")
print(f"  sum([1,2,3,4,5]) = {sum(nums)}")
print(f"  min([1,2,3,4,5]) = {min(nums)}")
print(f"  max([1,2,3,4,5]) = {max(nums)}")
print(f"  abs(-10) = {abs(-10)}")
print(f"  pow(2, 3) = {pow(2, 3)}")

print("\n" + "=" * 50)
print("演示完成！")
print("=" * 50)
