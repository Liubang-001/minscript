# Simple assert test

print("Testing assert")

x = 10
assert x == 10
print("assert x == 10 passed")

assert x > 5
print("assert x > 5 passed")

y = 20
assert y < 100, "y should be less than 100"
print("assert with message passed")

print("All assert tests passed!")
