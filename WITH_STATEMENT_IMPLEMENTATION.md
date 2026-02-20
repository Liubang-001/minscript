# With Statement Implementation

## Status: ✅ COMPLETE

The `with` statement (context manager protocol) has been successfully implemented.

## Implementation Details

### 1. Opcodes Added (src/vm/vm.h)
- `OP_CALL_ENTER`: Calls the `__enter__()` method on a context manager
- `OP_CALL_EXIT`: Calls the `__exit__(None, None, None)` method on a context manager

### 2. Parser Implementation (src/parser/parser.c)

The `with_statement()` function implements the context manager protocol:

```c
with expression as variable:
    block
```

**Implementation Strategy:**
1. Evaluate the context manager expression
2. Store the manager in a temporary global variable (`__with_manager_N__`)
3. Load the manager and call `__enter__()`
4. Store the result in the `as` variable (if provided)
5. Execute the with block
6. Load the manager from temporary variable
7. Call `__exit__(None, None, None)`

**Key Design Decision:**
- The manager is stored in a temporary global variable rather than kept on the stack
- This prevents stack corruption issues with nested with statements
- Each with statement gets a unique temporary variable name using a counter

### 3. VM Implementation (src/vm/vm.c)

#### OP_CALL_ENTER
- Takes a context manager from the stack (duplicated)
- Verifies it's an instance with an `__enter__` method
- Calls `__enter__()` with the instance as `self`
- Returns the result of `__enter__()` on the stack
- Stack: `[manager, manager]` → `[manager, enter_result]`

#### OP_CALL_EXIT
- Takes a context manager from the stack
- Verifies it's an instance with an `__exit__` method
- Calls `__exit__(None, None, None)` with the instance as `self`
- Pops the manager from the stack
- Stack: `[manager]` → `[]`

### 4. Context Manager Protocol

Classes can implement the context manager protocol by defining two methods:

```python
class MyContext:
    def __enter__(self):
        # Setup code
        return self  # or any value to bind to 'as' variable
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        # Cleanup code
        return nil  # or False to not suppress exceptions
```

## Features Supported

### ✅ Basic with statement
```python
with MyContext() as ctx:
    print(ctx)
```

### ✅ Without 'as' clause
```python
with MyContext():
    print("No variable binding")
```

### ✅ Nested with statements
```python
with Outer() as o:
    with Inner() as i:
        print(i)
    print(o)
```

### ✅ Multiple levels of nesting
```python
with A() as a:
    with B() as b:
        with C() as c:
            print(c)
```

### ✅ __enter__ returning different values
```python
class Counter:
    def __enter__(self):
        return 42  # Can return any value
```

## Limitations

### ❌ Exception handling in __exit__
- Currently, `__exit__` is always called with `(None, None, None)`
- Exception information is not passed to `__exit__`
- This requires full exception handling system to be implemented

### ❌ Exception suppression
- The return value of `__exit__` is currently ignored
- Cannot suppress exceptions by returning `True` from `__exit__`
- This also requires exception handling system

### ❌ Multiple context managers in one statement
```python
# Not supported yet:
with open("a.txt") as f1, open("b.txt") as f2:
    pass
```

## Test Files

- `test_with_simple.ms` - Basic with statement test
- `test_with_debug.ms` - Nested with statements test
- `test_with.ms` - Multiple test scenarios
- `test_with_comprehensive.ms` - Comprehensive test suite

## Test Results

All tests pass successfully:
- ✅ Basic context manager with 'as' clause
- ✅ Context manager without 'as' clause
- ✅ Nested with statements (2 levels)
- ✅ Multiple nested contexts (3 levels)
- ✅ File-like context manager
- ✅ Context manager returning different value from __enter__

## Usage Example

```python
class File:
    def __init__(self, filename, mode):
        self.filename = filename
        self.mode = mode
        self.closed = true
    
    def __enter__(self):
        print("Opening", self.filename)
        self.closed = false
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        print("Closing", self.filename)
        self.closed = true
        return nil
    
    def write(self, data):
        if not self.closed:
            print("Writing:", data)

# Usage
with File("data.txt", "w") as f:
    f.write("Hello")
    f.write("World")
# File is automatically closed here
```

## Implementation Time

Approximately 2-3 hours (including debugging and testing)

## Next Steps

To make the with statement fully functional:

1. **Exception Handling** - Implement try/except/finally to pass exception info to __exit__
2. **Exception Suppression** - Use the return value of __exit__ to suppress exceptions
3. **Multiple Context Managers** - Support `with a, b:` syntax
4. **Async Context Managers** - Implement `async with` for async/await support

## Comparison with Python

### Supported
- ✅ Basic with statement syntax
- ✅ Context manager protocol (__enter__ and __exit__)
- ✅ Nested with statements
- ✅ Optional 'as' clause

### Not Yet Supported
- ❌ Exception information in __exit__
- ❌ Exception suppression
- ❌ Multiple context managers in one statement
- ❌ Async context managers (async with)

## Conclusion

The with statement implementation is complete and functional for the most common use cases. It properly implements the context manager protocol and supports nested contexts. The main limitation is the lack of exception handling integration, which would require implementing the full exception system first.
