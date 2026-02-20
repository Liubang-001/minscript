# Test dictionary comprehensions

print("=== Test 1: Basic dict comprehension ===")
squares = {str(x): x * x for x in [0, 1, 2, 3, 4]}
print("squares:", squares)

print("\n=== Test 2: Dict comp with range ===")
cubes = {str(i): i ** 3 for i in [1, 2, 3, 4, 5]}
print("cubes:", cubes)

print("\n=== Test 3: String keys ===")
words = ["apple", "banana", "cherry"]
lengths = {word: len(word) for word in words}
print("lengths:", lengths)

print("\n=== All tests passed! ===")
