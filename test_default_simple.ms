# Simple test for default parameters

print("Test 1: Basic default parameter")
def add(a, b=10):
    result = a + b
    print(f"a={a}, b={b}, result={result}")
    return result

r1 = add(5)
r2 = add(5, 20)
print(f"r1={r1}, r2={r2}")

print()
print("Test 2: Multiple defaults")
def multiply(x, y=2, z=3):
    result = x * y * z
    print(f"x={x}, y={y}, z={z}, result={result}")
    return result

m1 = multiply(5)
m2 = multiply(5, 4)
m3 = multiply(5, 4, 2)
print(f"m1={m1}, m2={m2}, m3={m3}")

print()
print("Test 3: String defaults")
def say(message, prefix="INFO"):
    print(f"[{prefix}] {message}")

say("Hello")
say("Error occurred", "ERROR")

print()
print("Test 4: Error handling - too few args")
def require_two(a, b=10):
    return a + b

# This should work
result = require_two(5)
print(f"require_two(5) = {result}")

print()
print("All tests passed!")
