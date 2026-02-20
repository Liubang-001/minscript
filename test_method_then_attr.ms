# 方法然后属性

class Test:
    def __init__(self):
        self.value = 42
    
    def get_value(self):
        return self.value

t = Test()
print(t.get_value())
print(t.value)
