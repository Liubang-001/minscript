# Test with statement and context manager protocol

class MyContext:
    def __init__(self):
        self.name = "MyContext"
    
    def __enter__(self):
        print("Entering context:", self.name)
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        print("Exiting context:", self.name)
        return nil

# Test 1: Basic with statement with 'as' clause
print("Test 1: Basic with statement")
with MyContext() as ctx:
    print("Inside context, name:", ctx.name)

print()

# Test 2: With statement without 'as' clause
print("Test 2: Without 'as' clause")
with MyContext():
    print("Inside context without variable")

print()

# Test 3: Nested with statements
print("Test 3: Nested with statements")
class FileContext:
    def __init__(self, filename):
        self.filename = filename
    
    def __enter__(self):
        print("Opening file:", self.filename)
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        print("Closing file:", self.filename)
        return nil

with FileContext("outer.txt") as f1:
    print("In outer context")
    with FileContext("inner.txt") as f2:
        print("In inner context")
    print("Back to outer context")

print()
print("All tests completed!")
