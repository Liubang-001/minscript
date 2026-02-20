# 综合测试方法调用

class Calculator:
    def __init__(self, base):
        self.base = base
    
    def add(self, x):
        return self.base + x
    
    def multiply(self, x):
        return self.base * x

calc = Calculator(10)

# 测试1：直接在print中调用
print("Test 1:", calc.add(5))
print("Test 2:", calc.multiply(3))

# 测试2：多次调用
print(calc.add(1))
print(calc.add(2))
print(calc.add(3))

# 测试3：嵌套调用
print(calc.add(calc.multiply(2)))

# 测试4：赋值后调用
r1 = calc.add(7)
r2 = calc.multiply(4)
print("Results:", r1, r2)
