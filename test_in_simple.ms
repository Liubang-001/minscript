# 简单的 in 运算符测试

# 测试列表
nums = [1, 2, 3]
print("2 in nums:", 2 in nums)
print("4 in nums:", 4 in nums)

# 测试字符串
text = "hello world"
print("'hello' in text:", "hello" in text)
print("'xyz' in text:", "xyz" in text)

# 测试 __contains__
class MyList:
    def __init__(self):
        self.data = [10, 20, 30]
    
    def __contains__(self, item):
        print("Checking if", item, "is in MyList")
        for x in self.data:
            if x == item:
                return True
        return False

mylist = MyList()
print("20 in mylist:", 20 in mylist)
print("40 in mylist:", 40 in mylist)
