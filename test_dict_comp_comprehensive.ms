# Comprehensive dictionary comprehension tests

print("=== Test 1: Number to square ===")
squares = {str(x): x ** 2 for x in [0, 1, 2, 3, 4, 5]}
print("squares:", squares)

print("\n=== Test 2: Number to cube ===")
cubes = {str(i): i ** 3 for i in [1, 2, 3, 4, 5]}
print("cubes:", cubes)

print("\n=== Test 3: String to length ===")
words = ["apple", "banana", "cherry", "date"]
lengths = {word: len(word) for word in words}
print("lengths:", lengths)

print("\n=== Test 4: Number doubling ===")
doubled_vals = {str(n): n * 2 for n in [10, 20, 30]}
print("doubled_vals:", doubled_vals)

print("\n=== Test 5: Number to string ===")
num_to_str = {str(n): "Number " + str(n) for n in [1, 2, 3]}
print("num_to_str:", num_to_str)

print("\n=== Test 6: Expression in key and value ===")
doubled = {str(x * 2): x * 3 for x in [1, 2, 3, 4]}
print("doubled keys, tripled values:", doubled)

print("\n=== Test 7: Dict comp assigned then used ===")
temp_dict = {str(i): i for i in [1, 2, 3, 4, 5]}
result = len(temp_dict)
print("Length of dict comp:", result)

print("\n=== All tests passed! ===")
