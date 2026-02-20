# 测试扩展的魔术方法

class MyList:
    def __init__(self):
        self.items = []
    
    def append(self, item):
        self.items = self.items + [item]
    
    def __len__(self):
        return len(self.items)
    
    def __getitem__(self, index):
        return self.items[index]
    
    def __setitem__(self, index, value):
        # 需要手动更新列表
        temp = []
        i = 0
        for item in self.items:
            if i == index:
                temp = temp + [value]
            else:
                temp = temp + [item]
            i = i + 1
        self.items = temp
    
    def __str__(self):
        return f"MyList({self.items})"
    
    def __repr__(self):
        return f"MyList(items={self.items})"

# 测试
mylist = MyList()
mylist.append(1)
mylist.append(2)
mylist.append(3)

print("MyList:", mylist)
print("Length:", len(mylist))
print("Item 0:", mylist[0])
print("Item 1:", mylist[1])
print("Item 2:", mylist[2])

# 测试 __setitem__
mylist[1] = 99
print("After mylist[1] = 99:")
print("Item 1:", mylist[1])
print("MyList:", mylist)

# 测试 str() 和 repr()
print("str(mylist):", str(mylist))
# print("repr(mylist):", repr(mylist))  # repr() 函数可能未实现
