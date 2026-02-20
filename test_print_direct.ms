# 直接在print中调用

class Test:
    def get(self):
        return 1

t = Test()
print("First:", t.get())
print("Second:", t.get())
