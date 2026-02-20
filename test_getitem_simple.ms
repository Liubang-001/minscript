# 测试 __getitem__

class MyList:
    def __init__(self):
        self.items = [1, 2, 3]
    
    def __getitem__(self, index):
        print(f"__getitem__ called with index {index}")
        return self.items[index]
    
    def __len__(self):
        return len(self.items)

mylist = MyList()
print("Length:", len(mylist))
print("Item 0:", mylist[0])
print("Item 1:", mylist[1])
print("Item 2:", mylist[2])
