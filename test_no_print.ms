# 不使用print

class Test:
    def method1(self):
        return "method1"
    
    def method2(self):
        return "method2"

t = Test()
r1 = t.method1()
r2 = t.method2()
print(r1)
print(r2)
