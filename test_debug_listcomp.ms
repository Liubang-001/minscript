print("Test 1: Simple")
result1 = [x for x in [1, 2, 3]]
print(f"result1 = {result1}")
print(f"len(result1) = {len(result1)}")
if len(result1) > 0:
    print(f"result1[0] = {result1[0]}")
print()

print("Test 2: With expression")
result2 = [x * 2 for x in [4, 5, 6]]
print(f"result2 = {result2}")
print(f"len(result2) = {len(result2)}")
if len(result2) > 0:
    print(f"result2[0] = {result2[0]}")
    print(f"result2[1] = {result2[1]}")
    print(f"result2[2] = {result2[2]}")
print()

print("Done")
