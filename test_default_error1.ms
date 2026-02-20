# Test: Too few arguments
def needs_one(a, b=10):
    return a + b

# This should fail - no arguments provided
result = needs_one()
print(f"Result: {result}")
