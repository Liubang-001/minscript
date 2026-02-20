# MiniScript Feature Demo

print("=== Lambda ===")
add = lambda x, y: x + y
print("add(3, 5) =", add(3, 5))

print("\n=== List Comprehension ===")
squares = [x * x for x in [1, 2, 3, 4, 5]]
print("squares:", squares)

print("\n=== Default Parameters ===")
def greet(name, msg="Hello"):
    return msg + " " + name

print(greet("Alice"))
print(greet("Bob", "Hi"))

print("\n=== Classes ===")
class Vector:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    
    def __add__(self, other):
        return Vector(self.x + other.x, self.y + other.y)
    
    def __sub__(self, other):
        return Vector(self.x - other.x, self.y - other.y)
    
    def __mul__(self, n):
        return Vector(self.x * n, self.y * n)
    
    def __eq__(self, other):
        return self.x == other.x and self.y == other.y
    
    def __lt__(self, other):
        return (self.x * self.x + self.y * self.y) < (other.x * other.x + other.y * other.y)
    
    def __gt__(self, other):
        return (self.x * self.x + self.y * self.y) > (other.x * other.x + other.y * other.y)
    
    def __getitem__(self, i):
        if i == 0:
            return self.x
        return self.y

v1 = Vector(3, 4)
v2 = Vector(1, 2)
v3 = v1 + v2

print("v1 + v2 = (", v3.x, ",", v3.y, ")")
print("v1 == v2:", v1 == v2)
print("v1 > v2:", v1 > v2)
print("v1[0]:", v1[0])

print("\n=== Builtins ===")
nums = [1, 2, 3, 4, 5]
print("len:", len(nums))
print("sum:", sum(nums))
print("min:", min(nums))
print("max:", max(nums))

print("\n=== Done! ===")
