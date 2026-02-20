# 测试魔术方法

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
        # 比较向量的长度
        return (self.x * self.x + self.y * self.y) < (other.x * other.x + other.y * other.y)
    
    def __gt__(self, other):
        return (self.x * self.x + self.y * self.y) > (other.x * other.x + other.y * other.y)
    
    def __str__(self):
        return f"Vector({self.x}, {self.y})"

# 测试
v1 = Vector(3, 4)
v2 = Vector(1, 2)

print("v1 =", v1)
print("v2 =", v2)

# 测试加法
v3 = v1 + v2
print("v1 + v2 =", v3)

# 测试减法
v4 = v1 - v2
print("v1 - v2 =", v4)

# 测试乘法
v5 = v1 * 2
print("v1 * 2 =", v5)

# 测试相等
print("v1 == v2:", v1 == v2)
print("v1 == Vector(3, 4):", v1 == Vector(3, 4))

# 测试比较
print("v1 < v2:", v1 < v2)
print("v1 > v2:", v1 > v2)
print("v2 < v1:", v2 < v1)
print("v2 > v1:", v2 > v1)
