# 属性然后方法

class Test:
    def __init__(self):
        self.value = 42
    
    def get_value(self):
        return self.value

t = Test()
v = t.value
print(v)
r = t.get_value()
print(r)
