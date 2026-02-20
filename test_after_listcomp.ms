print("Test 1: Simple list comp")
result = [x for x in [1, 2, 3]]
print(f"result = {result}")
print()

print("Test 2: After list comp, use range")
for i in range(3):
    print(f"i = {i}")
print()

print("Test 3: List comp then for loop")
numbers = [1, 2, 3]
result2 = [x * 2 for x in numbers]
print(f"result2 = {result2}")
for i in range(3):
    print(f"i = {i}, result2[i] = {result2[i]}")
print()

print("Done")
