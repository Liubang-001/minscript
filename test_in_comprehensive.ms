# Comprehensive test for in operator and __contains__

print("=== Test 1: List membership ===")
nums = [1, 2, 3, 4, 5]
print("2 in [1,2,3,4,5]:", 2 in nums)
print("6 in [1,2,3,4,5]:", 6 in nums)
print("1 in [1,2,3,4,5]:", 1 in nums)
print("5 in [1,2,3,4,5]:", 5 in nums)

print("\n=== Test 2: Tuple membership ===")
colors = ("red", "green", "blue")
print("'red' in tuple:", "red" in colors)
print("'yellow' in tuple:", "yellow" in colors)

print("\n=== Test 3: String contains ===")
text = "hello world"
print("'hello' in 'hello world':", "hello" in text)
print("'world' in 'hello world':", "world" in text)
print("'xyz' in 'hello world':", "xyz" in text)
print("'o w' in 'hello world':", "o w" in text)

print("\n=== Test 4: Dictionary key membership ===")
person = {"name": "Alice", "age": 30}
print("'name' in dict:", "name" in person)
print("'age' in dict:", "age" in person)
print("'email' in dict:", "email" in person)

print("\n=== Test 5: __contains__ magic method ===")
class Range:
    def __init__(self, start, end):
        self.start = start
        self.end = end
    
    def __contains__(self, value):
        return value >= self.start and value < self.end

r = Range(10, 20)
print("15 in Range(10, 20):", 15 in r)
print("5 in Range(10, 20):", 5 in r)
print("20 in Range(10, 20):", 20 in r)
print("10 in Range(10, 20):", 10 in r)

print("\n=== Test 6: in operator in expressions ===")
result = (3 in [1, 2, 3]) and (4 in [4, 5, 6])
print("(3 in [1,2,3]) and (4 in [4,5,6]):", result)

result = (10 in [1, 2, 3]) or (4 in [4, 5, 6])
print("(10 in [1,2,3]) or (4 in [4,5,6]):", result)

if 2 in [1, 2, 3]:
    print("2 is in the list!")

print("\n=== Test 7: in with for loop (no conflict) ===")
for x in [1, 2, 3]:
    if x in [2, 3, 4]:
        print(f"{x} is in both lists")

print("\n=== All tests passed! ===")
