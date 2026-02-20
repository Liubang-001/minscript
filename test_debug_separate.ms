# 分开测试

class Test:
    def __init__(self, value):
        self.value = value
    
    def get_value(self):
        return self.value

t = Test(42)

# 只访问属性
print(t.value)

# 创建新实例
t2 = Test(99)

# 只调用方法
print(t2.get_value())
