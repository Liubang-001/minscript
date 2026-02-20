# Debug nested with statements

class FileContext:
    def __init__(self, filename):
        self.filename = filename
    
    def __enter__(self):
        print("Opening file:", self.filename)
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        print("Closing file:", self.filename)
        return nil

print("Creating outer context")
outer = FileContext("outer.txt")

print("Entering with statement for outer")
with outer as f1:
    print("In outer context, filename:", f1.filename)
    
    print("Creating inner context")
    inner = FileContext("inner.txt")
    
    print("Entering with statement for inner")
    with inner as f2:
        print("In inner context, filename:", f2.filename)
    
    print("Back to outer context after inner with")

print("After outer with statement")
