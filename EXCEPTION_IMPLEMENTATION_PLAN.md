# Exception Handling Implementation Plan

## Overview
Implement Python-style exception handling with try/except/finally blocks.

## Phase 1: Foundation (2-3 hours)

### 1.1 Exception Value Type
Add to `include/miniscript.h`:
```c
typedef struct {
    char* type;        // Exception type (ValueError, TypeError, etc.)
    char* message;     // Error message
    int line;          // Line where exception occurred
} ms_exception_t;

// Add to ms_value_type enum:
MS_VAL_EXCEPTION
```

### 1.2 Exception Opcodes
Add to `src/vm/vm.h`:
```c
OP_TRY_BEGIN,      // Mark try block start, push exception handler
OP_TRY_END,        // Mark try block end, pop exception handler
OP_RAISE,          // Raise an exception
OP_JUMP_IF_EXCEPTION,  // Jump if there's an active exception
```

### 1.3 Exception Handler Stack
Add to VM structure in `src/vm/vm.h`:
```c
typedef struct {
    uint8_t* handler_ip;   // Jump target for exception handler
    int stack_height;      // Stack height to restore
    int frame_index;       // Frame index when handler was pushed
} ms_exception_handler_t;

// In ms_vm struct:
ms_exception_handler_t exception_handlers[64];
int exception_handler_count;
ms_value_t current_exception;  // Currently active exception
bool has_exception;            // Whether an exception is active
```

## Phase 2: Parser Implementation (3-4 hours)

### 2.1 Try Statement Parser
```c
static void try_statement(ms_parser_t* parser) {
    // try:
    //     risky_code()
    // except ExceptionType as e:
    //     handle_error(e)
    // except:
    //     handle_any()
    // finally:
    //     cleanup()
}
```

### 2.2 Raise Statement Parser
```c
static void raise_statement(ms_parser_t* parser) {
    // raise ExceptionType("message")
    // raise  # re-raise current exception
}
```

## Phase 3: VM Implementation (3-4 hours)

### 3.1 OP_TRY_BEGIN
- Push exception handler onto handler stack
- Store current IP, stack height, frame index

### 3.2 OP_RAISE
- Create exception object
- Set has_exception flag
- Unwind stack to nearest exception handler
- Jump to handler IP

### 3.3 Exception Unwinding
- Pop stack to handler's stack height
- Restore frame if needed
- Jump to except block

### 3.4 OP_TRY_END
- Pop exception handler from stack
- Continue normal execution

## Phase 4: Built-in Exception Classes (1-2 hours)

Create exception hierarchy:
```
Exception
  ├─ ValueError
  ├─ TypeError
  ├─ KeyError
  ├─ IndexError
  ├─ AttributeError
  ├─ ZeroDivisionError
  └─ RuntimeError
```

## Phase 5: Integration (1-2 hours)

### 5.1 Update with statement
- Pass exception info to __exit__
- Handle exception suppression

### 5.2 Update runtime errors
- Convert runtime_error() calls to raise exceptions
- Ensure proper exception propagation

## Testing Strategy

### Test 1: Basic try/except
```python
try:
    x = 10 / 0
except ZeroDivisionError:
    print("Caught division by zero")
```

### Test 2: Multiple except blocks
```python
try:
    risky_operation()
except ValueError:
    print("Value error")
except TypeError:
    print("Type error")
```

### Test 3: Exception with message
```python
try:
    raise ValueError("Invalid value")
except ValueError as e:
    print("Error:", e)
```

### Test 4: Finally block
```python
try:
    risky_operation()
except:
    print("Error occurred")
finally:
    print("Cleanup")
```

### Test 5: Nested try/except
```python
try:
    try:
        raise ValueError("inner")
    except TypeError:
        pass
except ValueError:
    print("Caught in outer")
```

### Test 6: Re-raise
```python
try:
    raise ValueError("test")
except ValueError:
    print("Caught")
    raise  # re-raise
```

## Implementation Order

1. ✅ With statement (COMPLETE)
2. ⏳ Exception handling (IN PROGRESS)
   - [ ] Phase 1: Foundation
   - [ ] Phase 2: Parser
   - [ ] Phase 3: VM
   - [ ] Phase 4: Built-in exceptions
   - [ ] Phase 5: Integration
3. ⏸️ Decorator improvements (DEFERRED)

## Estimated Time: 10-15 hours

## Priority: HIGH (marked as "很重要" by user)
