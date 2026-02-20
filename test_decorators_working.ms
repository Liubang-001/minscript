# Test working decorators

print("=== Test 1: Identity decorator ===")

def identity(f):
    print(f"Decorating function")
    return f

@identity
def hello():
    print("Hello!")

hello()

print("\n=== Test 2: Wrapper decorator ===")

def make_bold(func):
    def wrapper():
        print("**BOLD START**")
        func()
        print("**BOLD END**")
    return wrapper

@make_bold
def greet():
    print("Hello, World!")

greet()

print("\n=== Test 3: Multiple decorators ===")

def dec1(f):
    print("Applying decorator 1")
    def wrapper():
        print("[DEC1 START]")
        f()
        print("[DEC1 END]")
    return wrapper

def dec2(f):
    print("Applying decorator 2")
    def wrapper():
        print("[DEC2 START]")
        f()
        print("[DEC2 END]")
    return wrapper

@dec1
@dec2
def multi():
    print("Multi-decorated function")

print("Calling multi-decorated function:")
multi()

print("\n=== All tests passed! ===")
