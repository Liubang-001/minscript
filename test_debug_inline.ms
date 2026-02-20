# 调试内联调用

class Test:
    def __init__(self, value):
        self.value = value
    
    def get_value(self):
        return self.value

t = Test(42)
print(t.value)
print(t.get_value())
