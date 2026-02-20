# Simplest decorator test

def identity(f):
    print("Decorator called with:", f)
    return f

@identity
def test():
    print("Test function")

print("Calling test...")
test()
