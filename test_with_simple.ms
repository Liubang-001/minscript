# Simple test for with statement

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

print("After with statement")
