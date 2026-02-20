# 测试没有 __init__ 的多个方法

class Test:
    def method1(self):
        return "method1"
    
    def method2(self):
        return "method2"

t = Test()
print(t.method1())
print(t.method2())
