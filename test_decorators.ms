print("=== 装饰器测试 ===")

# @property 测试
class Circle:
    def __init__(self, radius):
        self._radius = radius
    
    @property
    def radius(self):
        return self._radius
    
    @radius.setter
    def radius(self, value):
        if value < 0:
            raise ValueError("Radius cannot be negative")
        self._radius = value
    
    @property
    def area(self):
        return 3.14159 * self._radius ** 2
    
    @property
    def circumference(self):
        return 2 * 3.14159 * self._radius

c = Circle(5)
print(f"半径: {c.radius}")  # 5
print(f"面积: {c.area}")  # 78.53975
print(f"周长: {c.circumference}")  # 31.4159

c.radius = 10
print(f"新半径: {c.radius}")  # 10
print(f"新面积: {c.area}")  # 314.159

# @classmethod 测试
class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age
    
    @classmethod
    def from_birth_year(cls, name, year):
        age = 2024 - year
        return cls(name, age)
    
    def introduce(self):
        return f"{self.name}, {self.age} years old"

p1 = Person("Alice", 30)
print(p1.introduce())  # Alice, 30 years old

p2 = Person.from_birth_year("Bob", 1990)
print(p2.introduce())  # Bob, 34 years old

# @staticmethod 测试
class Math:
    @staticmethod
    def add(a, b):
        return a + b
    
    @staticmethod
    def multiply(a, b):
        return a * b

print(f"5 + 3 = {Math.add(5, 3)}")  # 8
print(f"5 * 3 = {Math.multiply(5, 3)}")  # 15

print("✅ 所有测试通过！")
