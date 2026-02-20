# 有两个方法但只调用一个

class Test:
    def method1(self):
        return "method1"
    
    def method2(self):
        return "method2"

t = Test()
print(t.method1())
