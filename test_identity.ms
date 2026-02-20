# 测试identity函数

def identity(x):
    return x

class Test:
    def method1(self):
        return "method1"
    
    def method2(self):
        return "method2"

t = Test()
print(identity(t.method1()))
print(identity(t.method2()))
