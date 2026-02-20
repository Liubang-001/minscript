# 测试 __le__ 和 __ge__ 魔术方法

class Number:
    def __init__(self, value):
        self.value = value
    
    def __lt__(self, other):
        print(f"__lt__ called: {self.value} < {other.value}")
        return self.value < other.value
    
    def __le__(self, other):
        print(f"__le__ called: {self.value} <= {other.value}")
        return self.value <= other.value
    
    def __gt__(self, other):
        print(f"__gt__ called: {self.value} > {other.value}")
        return self.value > other.value
    
    def __ge__(self, other):
        print(f"__ge__ called: {self.value} >= {other.value}")
        return self.value >= other.value

n1 = Number(5)
n2 = Number(10)
n3 = Number(5)

print("Testing <=:")
print("n1 <= n2:", n1 <= n2)
print("n1 <= n3:", n1 <= n3)
print("n2 <= n1:", n2 <= n1)

print("\nTesting >=:")
print("n1 >= n2:", n1 >= n2)
print("n1 >= n3:", n1 >= n3)
print("n2 >= n1:", n2 >= n1)
