# Test basic decorator functionality

print("=== Test 1: Simple decorator ===")

def my_decorator(func):
    print(f"Decorating {func}")
    return func

@my_decorator
def hello():
    print("Hello from decorated function!")

hello()

print("\n=== Test 2: Decorator that wraps ===")

def uppercase_decorator(func):
    def wrapper():
        result = func()
        return result
    return wrapper

@uppercase_decorator
def greet():
    print("Hello, World!")
    return "done"

result = greet()
print("Result:", result)

print("\n=== Test 3: Multiple decorators ===")

def decorator1(func):
    print("Applying decorator1")
    return func

def decorator2(func):
    print("Applying decorator2")
    return func

@decorator1
@decorator2
def multi_decorated():
    print("Multi-decorated function")

multi_decorated()

print("\n=== All tests passed! ===")
