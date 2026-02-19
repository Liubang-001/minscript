def test():
    a = 1

test()


# break / continue
for i in range(5):
    if i == 2:
        continue          # 跳过本次循环
    if i == 4:
        break             # 终止循环
    print(f"i = {i}")