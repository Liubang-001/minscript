# 简单继承测试

class Parent:
    def __init__(self):
        self.x = 10
    
    def show(self):
        print("Parent show")
        print(self.x)

class Child(Parent):
    def __init__(self):
        self.x = 20
        self.y = 30
    
    def show_child(self):
        print("Child show")
        print(self.y)

# 测试
p = Parent()
p.show()

c = Child()
c.show()
c.show_child()
