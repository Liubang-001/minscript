print("=== self 测试 ===")

class Person:
    def __init__(self, name):
        self.name = name
    
    def test(self):
        print("In test method")
        print(self)
        n = self.name
        print(n)
        return n

p = Person("Alice")
result = p.test()
print(result)
