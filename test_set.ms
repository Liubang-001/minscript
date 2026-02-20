# Test set functionality

print("=== Test 1: Create empty set ===")
s = set()
print("Empty set:", s)

print("\n=== Test 2: Create set from list ===")
s = set([1, 2, 3, 2, 1])
print("set([1, 2, 3, 2, 1]):", s)

print("\n=== Test 3: Create set from tuple ===")
s = set((4, 5, 6, 5, 4))
print("set((4, 5, 6, 5, 4)):", s)

print("\n=== Test 4: Set with strings ===")
s = set(["apple", "banana", "apple"])
print("set with strings:", s)

print("\n=== Test 5: Set with mixed types ===")
s = set([1, "two", 3, "two", 1])
print("set with mixed types:", s)

print("\n=== All tests passed! ===")
