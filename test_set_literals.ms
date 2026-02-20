# Test set literals and comprehensions

print("=== Test 1: Set literal ===")
s = {1, 2, 3}
print("Set literal {1, 2, 3}:", s)

print("\n=== Test 2: Set literal with duplicates ===")
s = {1, 2, 3, 2, 1}
print("Set {1, 2, 3, 2, 1}:", s)

print("\n=== Test 3: Set with strings ===")
s = {"apple", "banana", "cherry"}
print("String set:", s)

print("\n=== Test 4: Set comprehension ===")
s = {x * 2 for x in [1, 2, 3, 4, 5]}
print("Set comp {x*2 for x in [1,2,3,4,5]}:", s)

print("\n=== Test 5: Set comprehension with duplicates ===")
s = {x % 3 for x in [1, 2, 3, 4, 5, 6, 7, 8, 9]}
print("Set comp {x%3 for x in range(1,10)}:", s)

print("\n=== All tests passed! ===")
