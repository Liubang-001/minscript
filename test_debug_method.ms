# 调试方法调用

class Test:
    def get(self):
        print("In get method")
        return 1

t = Test()
print("First call:")
r1 = t.get()
print(r1)

print("Second call:")
r2 = t.get()
print(r2)
