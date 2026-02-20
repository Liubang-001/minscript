# Test for loop with in operator inside
print("=== For loop with in operator ===")
for x in [1, 2, 3]:
    print(f"x = {x}")
    result = x in [2, 3, 4]
    print(f"  {x} in [2,3,4]: {result}")
