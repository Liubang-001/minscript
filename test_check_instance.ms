# 检查实例是否被破坏

class Test:
    def __init__(self):
        self.value = 42
    
    def get_value(self):
        return self.value

t = Test()
print("Before first call:")
print(t.value)

print("First call:")
print(t.get_value())

print("After first call:")
print(t.value)

print("Second call:")
print(t.get_value())
