# 调试属性访问

class Test:
    def __init__(self, value):
        self.value = value
    
    def get_value(self):
        return self.value

print("Creating instance...")
t = Test(42)

print("Accessing attribute...")
v = t.value
print(v)

print("Calling method...")
result = t.get_value()
print(result)

print("Done!")
