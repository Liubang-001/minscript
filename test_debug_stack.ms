print("Before list comp")
result = [x for x in [1, 2, 3]]
print("After list comp, before for")
print(result)
for i in range(3):
    print(i)
