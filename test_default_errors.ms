# Test error handling for default parameters

print("Test 1: Too few arguments")
def needs_one(a, b=10):
    return a + b

# This should fail - no arguments provided
try:
    result = needs_one()
except:
    print("Error: needs_one() requires at least 1 argument")

print()
print("Test 2: Too many arguments")
def takes_two(a, b=10):
    return a + b

# This should fail - too many arguments
try:
    result = takes_two(1, 2, 3)
except:
    print("Error: takes_two() takes at most 2 arguments")

print()
print("All error tests completed!")
