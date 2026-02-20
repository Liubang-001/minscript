# Test: Too many arguments
def takes_two(a, b=10):
    return a + b

# This should fail - too many arguments
result = takes_two(1, 2, 3)
print(f"Result: {result}")
