# 测试只调用第二个方法

class Test:
    def method1(self):
        return "method1"
    
    def method2(self):
        return "method2"

t = Test()
print(t.method2())
