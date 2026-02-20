# Comprehensive test for with statement

print("=== Test 1: Basic context manager ===")
class MyContext:
    def __init__(self, name):
        self.name = name
    
    def __enter__(self):
        print("Entering:", self.name)
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        print("Exiting:", self.name)
        return nil

with MyContext("test1") as ctx:
    print("Inside:", ctx.name)

print()

print("=== Test 2: Without 'as' clause ===")
with MyContext("test2"):
    print("Inside without variable")

print()

print("=== Test 3: Nested with statements ===")
with MyContext("outer") as outer:
    print("In outer:", outer.name)
    with MyContext("inner") as inner:
        print("In inner:", inner.name)
    print("Back in outer:", outer.name)

print()

print("=== Test 4: File-like context manager ===")
class File:
    def __init__(self, filename, mode):
        self.filename = filename
        self.mode = mode
        self.closed = true
    
    def __enter__(self):
        print("Opening", self.filename, "in mode", self.mode)
        self.closed = false
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        print("Closing", self.filename)
        self.closed = true
        return nil
    
    def write(self, data):
        if self.closed:
            print("Error: File is closed")
        else:
            print("Writing to", self.filename + ":", data)

with File("data.txt", "w") as f:
    f.write("Hello")
    f.write("World")

print()

print("=== Test 5: Multiple nested contexts ===")
with MyContext("A") as a:
    print("Level A")
    with MyContext("B") as b:
        print("Level B")
        with MyContext("C") as c:
            print("Level C")
        print("Back to B")
    print("Back to A")

print()

print("=== Test 6: Context manager returning different value ===")
class Counter:
    def __init__(self):
        self.count = 0
    
    def __enter__(self):
        print("Counter starting")
        return 42  # Return a different value
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        print("Counter ending")
        return nil

with Counter() as value:
    print("Value from __enter__:", value)

print()

print("=== All tests passed! ===")
