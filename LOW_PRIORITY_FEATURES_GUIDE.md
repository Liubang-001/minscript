# 低优先级功能实现指南

这些功能虽然是Python的重要特性，但对于基础解释器来说不是必需的。它们的实现复杂度较高，需要对VM进行较大的修改。

## 1. 异常处理 (try/except/finally)

### 难度：⭐⭐⭐⭐ 非常复杂
### 预计时间：8-12小时

### 概述
异常处理是Python的核心特性之一，需要实现完整的异常传播和捕获机制。

### 需要的组件

#### 1.1 异常对象系统
```c
// 在 value.h 中
typedef struct {
    char* type;        // 异常类型 (ValueError, TypeError等)
    char* message;     // 错误消息
    int line;          // 发生位置
    char* traceback;   // 调用栈追踪
} ms_exception_t;

// 新的值类型
MS_VAL_EXCEPTION
```

#### 1.2 新的操作码
```c
OP_TRY_BEGIN,      // 标记try块开始
OP_TRY_END,        // 标记try块结束
OP_EXCEPT_BEGIN,   // 标记except块开始
OP_EXCEPT_END,     // 标记except块结束
OP_FINALLY_BEGIN,  // 标记finally块开始
OP_FINALLY_END,    // 标记finally块结束
OP_RAISE,          // 抛出异常
OP_RERAISE,        // 重新抛出异常
```

#### 1.3 异常处理栈
```c
typedef struct {
    uint8_t* try_ip;       // try块的指令指针
    uint8_t* except_ip;    // except块的指令指针
    uint8_t* finally_ip;   // finally块的指令指针
    int stack_height;      // 栈高度
} ms_exception_handler_t;

// 在 VM 中
ms_exception_handler_t exception_handlers[64];
int exception_handler_count;
ms_exception_t* current_exception;
```

### 实现步骤

#### 步骤1：词法分析器 (30分钟)
```c
// 已有的TOKEN
TOKEN_TRY,
TOKEN_EXCEPT,
TOKEN_FINALLY,
TOKEN_RAISE,
```

#### 步骤2：解析器 (3-4小时)
```c
static void try_statement(ms_parser_t* parser) {
    // try:
    //     risky_code()
    // except ValueError as e:
    //     handle_error(e)
    // except:
    //     handle_any_error()
    // finally:
    //     cleanup()
    
    consume(parser, TOKEN_TRY, "Expect 'try'.");
    consume(parser, TOKEN_COLON, "Expect ':' after 'try'.");
    
    // 发出 OP_TRY_BEGIN
    emit_byte(parser, OP_TRY_BEGIN);
    int try_jump = emit_jump(parser, OP_JUMP);  // 跳过异常处理代码
    
    // 编译try块
    block(parser);
    
    emit_byte(parser, OP_TRY_END);
    
    // 编译except块
    while (match(parser, TOKEN_EXCEPT)) {
        // except ExceptionType as var:
        if (match(parser, TOKEN_IDENTIFIER)) {
            // 异常类型
        }
        if (match(parser, TOKEN_AS)) {
            // 异常变量
        }
        consume(parser, TOKEN_COLON, "Expect ':' after except.");
        block(parser);
    }
    
    // 编译finally块
    if (match(parser, TOKEN_FINALLY)) {
        consume(parser, TOKEN_COLON, "Expect ':' after 'finally'.");
        emit_byte(parser, OP_FINALLY_BEGIN);
        block(parser);
        emit_byte(parser, OP_FINALLY_END);
    }
    
    patch_jump(parser, try_jump);
}
```

#### 步骤3：VM实现 (4-6小时)
```c
case OP_TRY_BEGIN: {
    // 保存当前状态
    ms_exception_handler_t* handler = &vm->exception_handlers[vm->exception_handler_count++];
    handler->try_ip = frame->ip;
    handler->stack_height = vm->stack_top - vm->stack;
    // 读取except和finally的跳转地址
    break;
}

case OP_RAISE: {
    // 创建异常对象
    ms_value_t exception_val = ms_vm_pop(vm);
    
    // 查找异常处理器
    while (vm->exception_handler_count > 0) {
        ms_exception_handler_t* handler = &vm->exception_handlers[--vm->exception_handler_count];
        
        // 恢复栈
        vm->stack_top = vm->stack + handler->stack_height;
        
        // 跳转到except块
        if (handler->except_ip) {
            frame->ip = handler->except_ip;
            ms_vm_push(vm, exception_val);
            break;
        }
        
        // 执行finally块
        if (handler->finally_ip) {
            frame->ip = handler->finally_ip;
        }
    }
    
    // 如果没有处理器，程序终止
    if (vm->exception_handler_count == 0) {
        runtime_error(vm, "Unhandled exception");
        return MS_RESULT_RUNTIME_ERROR;
    }
    break;
}
```

#### 步骤4：内置异常类 (1-2小时)
```python
# 需要实现的异常类
Exception
  ├─ ValueError
  ├─ TypeError
  ├─ KeyError
  ├─ IndexError
  ├─ AttributeError
  ├─ ZeroDivisionError
  └─ RuntimeError
```

### 测试用例
```python
# test_exceptions.ms
try:
    x = 10 / 0
except ZeroDivisionError as e:
    print("Caught:", e)
finally:
    print("Cleanup")

try:
    raise ValueError("Invalid value")
except ValueError as e:
    print("Error:", e)
```

---

## 2. with语句 (上下文管理器)

### 难度：⭐⭐⭐ 复杂
### 预计时间：4-6小时

### 概述
with语句需要实现上下文管理器协议（__enter__和__exit__魔术方法）。

### 需要的组件

#### 2.1 新的操作码
```c
OP_WITH_BEGIN,     // 调用 __enter__
OP_WITH_END,       // 调用 __exit__
OP_WITH_CLEANUP,   // 清理资源
```

#### 2.2 魔术方法
```python
class File:
    def __enter__(self):
        # 获取资源
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        # 释放资源
        self.close()
        return False  # 不抑制异常
```

### 实现步骤

#### 步骤1：解析器 (2小时)
```c
static void with_statement(ms_parser_t* parser) {
    // with expression as variable:
    //     block
    
    consume(parser, TOKEN_WITH, "Expect 'with'.");
    
    // 编译表达式
    expression(parser);
    
    // 调用 __enter__
    emit_byte(parser, OP_WITH_BEGIN);
    
    // as 变量
    if (match(parser, TOKEN_AS)) {
        consume(parser, TOKEN_IDENTIFIER, "Expect variable name.");
        uint8_t var = add_name(parser->previous.start, parser->previous.length);
        define_variable(parser, var);
    }
    
    consume(parser, TOKEN_COLON, "Expect ':' after with.");
    
    // 编译块
    block(parser);
    
    // 调用 __exit__
    emit_byte(parser, OP_WITH_END);
}
```

#### 步骤2：VM实现 (2-3小时)
```c
case OP_WITH_BEGIN: {
    ms_value_t context = peek(vm, 0);
    
    // 检查是否有 __enter__ 方法
    if (ms_value_is_instance(context)) {
        ms_instance_t* instance = ms_value_as_instance(context);
        
        if (ms_dict_has(instance->klass->methods, "__enter__")) {
            ms_value_t enter_method = ms_dict_get(instance->klass->methods, "__enter__");
            
            // 调用 __enter__
            // ... (类似其他魔术方法的调用)
        }
    }
    break;
}

case OP_WITH_END: {
    ms_value_t context = peek(vm, 0);
    
    // 调用 __exit__(None, None, None)
    if (ms_value_is_instance(context)) {
        ms_instance_t* instance = ms_value_as_instance(context);
        
        if (ms_dict_has(instance->klass->methods, "__exit__")) {
            ms_value_t exit_method = ms_dict_get(instance->klass->methods, "__exit__");
            
            // 调用 __exit__
            // ... 
        }
    }
    break;
}
```

### 测试用例
```python
# test_with.ms
class MyContext:
    def __enter__(self):
        print("Entering context")
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        print("Exiting context")
        return False

with MyContext() as ctx:
    print("Inside context")
```

---

## 3. yield (生成器)

### 难度：⭐⭐⭐⭐⭐ 极其复杂
### 预计时间：12-20小时

### 概述
生成器需要实现协程机制，能够暂停和恢复函数执行。这是最复杂的特性之一。

### 需要的组件

#### 3.1 生成器对象
```c
typedef struct {
    ms_function_t* function;   // 生成器函数
    ms_call_frame_t frame;     // 保存的调用帧
    ms_value_t* saved_stack;   // 保存的栈
    int saved_stack_size;      // 栈大小
    uint8_t* resume_ip;        // 恢复执行的位置
    bool finished;             // 是否完成
} ms_generator_t;

MS_VAL_GENERATOR
```

#### 3.2 新的操作码
```c
OP_YIELD,          // yield 表达式
OP_YIELD_FROM,     // yield from 表达式
OP_GENERATOR,      // 创建生成器
```

#### 3.3 迭代器协议
```python
class Generator:
    def __iter__(self):
        return self
    
    def __next__(self):
        # 恢复执行直到下一个yield
        pass
```

### 实现步骤

#### 步骤1：识别生成器函数 (2小时)
```c
// 在解析函数时，检查是否包含yield
static bool contains_yield(ms_parser_t* parser) {
    // 扫描函数体，查找yield关键字
}

static void function_declaration(ms_parser_t* parser) {
    // ...
    if (contains_yield(parser)) {
        // 标记为生成器函数
        function->is_generator = true;
    }
}
```

#### 步骤2：yield实现 (4-6小时)
```c
case OP_YIELD: {
    // 保存当前状态
    ms_generator_t* gen = current_generator;
    
    // 保存栈
    gen->saved_stack_size = vm->stack_top - vm->stack;
    gen->saved_stack = malloc(sizeof(ms_value_t) * gen->saved_stack_size);
    memcpy(gen->saved_stack, vm->stack, gen->saved_stack_size * sizeof(ms_value_t));
    
    // 保存IP
    gen->resume_ip = frame->ip;
    
    // 返回yield的值
    ms_value_t yield_value = ms_vm_pop(vm);
    return yield_value;
}
```

#### 步骤3：生成器调用 (4-6小时)
```c
// 调用生成器函数时，返回生成器对象而不是执行
if (function->is_generator) {
    ms_generator_t* gen = ms_generator_new(function);
    ms_vm_push(vm, ms_value_generator(gen));
} else {
    // 正常函数调用
}
```

#### 步骤4：__next__实现 (2-4小时)
```c
// 实现生成器的__next__方法
ms_value_t generator_next(ms_generator_t* gen) {
    if (gen->finished) {
        // 抛出 StopIteration
        return ms_value_nil();
    }
    
    // 恢复栈
    memcpy(vm->stack, gen->saved_stack, gen->saved_stack_size * sizeof(ms_value_t));
    vm->stack_top = vm->stack + gen->saved_stack_size;
    
    // 恢复IP
    frame->ip = gen->resume_ip;
    
    // 继续执行直到下一个yield
    ms_result_t result = run(vm);
    
    // 返回yield的值
    return result_value;
}
```

### 测试用例
```python
# test_generator.ms
def count_up_to(n):
    i = 0
    while i < n:
        yield i
        i = i + 1

gen = count_up_to(5)
for num in gen:
    print(num)  # 输出: 0, 1, 2, 3, 4
```

---

## 4. async/await (异步编程)

### 难度：⭐⭐⭐⭐⭐⭐ 最复杂
### 预计时间：20-30小时

### 概述
异步编程需要实现事件循环、协程、Future对象等复杂机制。这是Python中最复杂的特性。

### 需要的组件

#### 4.1 事件循环
```c
typedef struct {
    ms_coroutine_t** coroutines;  // 协程队列
    int coroutine_count;
    bool running;
} ms_event_loop_t;
```

#### 4.2 协程对象
```c
typedef struct {
    ms_function_t* function;
    ms_call_frame_t frame;
    ms_value_t* saved_stack;
    uint8_t* resume_ip;
    ms_value_t awaiting;  // 正在等待的Future
    bool finished;
} ms_coroutine_t;

MS_VAL_COROUTINE
```

#### 4.3 Future对象
```c
typedef struct {
    ms_value_t result;
    bool done;
    ms_coroutine_t** waiting;  // 等待此Future的协程
    int waiting_count;
} ms_future_t;

MS_VAL_FUTURE
```

#### 4.4 新的操作码
```c
OP_ASYNC_DEF,      // 定义异步函数
OP_AWAIT,          // await 表达式
OP_ASYNC_FOR,      // async for
OP_ASYNC_WITH,     // async with
```

### 实现步骤

#### 步骤1：事件循环 (4-6小时)
```c
ms_event_loop_t* ms_event_loop_new() {
    ms_event_loop_t* loop = malloc(sizeof(ms_event_loop_t));
    loop->coroutines = NULL;
    loop->coroutine_count = 0;
    loop->running = false;
    return loop;
}

void ms_event_loop_run(ms_event_loop_t* loop) {
    loop->running = true;
    
    while (loop->running && loop->coroutine_count > 0) {
        // 执行所有就绪的协程
        for (int i = 0; i < loop->coroutine_count; i++) {
            ms_coroutine_t* coro = loop->coroutines[i];
            
            if (coro->awaiting == NULL || is_ready(coro->awaiting)) {
                // 恢复协程执行
                resume_coroutine(coro);
            }
        }
    }
}
```

#### 步骤2：async def (4-6小时)
```c
static void async_function_declaration(ms_parser_t* parser) {
    consume(parser, TOKEN_ASYNC, "Expect 'async'.");
    consume(parser, TOKEN_DEF, "Expect 'def' after 'async'.");
    
    // 类似普通函数，但标记为异步
    function->is_async = true;
}
```

#### 步骤3：await实现 (6-8小时)
```c
case OP_AWAIT: {
    ms_value_t awaitable = ms_vm_pop(vm);
    
    if (ms_value_is_coroutine(awaitable)) {
        ms_coroutine_t* coro = ms_value_as_coroutine(awaitable);
        
        // 如果协程未完成，暂停当前协程
        if (!coro->finished) {
            // 保存当前状态
            current_coroutine->awaiting = awaitable;
            current_coroutine->resume_ip = frame->ip;
            
            // 返回到事件循环
            return MS_RESULT_YIELD;
        }
        
        // 协程已完成，获取结果
        ms_vm_push(vm, coro->result);
    }
    break;
}
```

#### 步骤4：异步迭代器 (4-6小时)
```python
class AsyncIterator:
    async def __aiter__(self):
        return self
    
    async def __anext__(self):
        # 返回下一个值或抛出StopAsyncIteration
        pass
```

### 测试用例
```python
# test_async.ms
async def fetch_data(url):
    print(f"Fetching {url}")
    await sleep(1)  # 模拟异步操作
    return f"Data from {url}"

async def main():
    result = await fetch_data("http://example.com")
    print(result)

# 运行事件循环
run(main())
```

---

## 实现优先级建议

### 如果要实现这些功能，建议顺序：

1. **with语句** (4-6小时) - 相对简单，有实用价值
2. **异常处理** (8-12小时) - 重要但复杂
3. **生成器** (12-20小时) - 非常复杂
4. **async/await** (20-30小时) - 最复杂，需要前面的基础

### 总时间估计：44-68小时

---

## 为什么这些是低优先级？

1. **复杂度高** - 需要对VM进行大量修改
2. **依赖关系** - async/await依赖于生成器，生成器依赖于迭代器
3. **使用频率** - 对于基础脚本语言，这些特性不是必需的
4. **测试难度** - 需要大量的测试用例来验证正确性

---

## 建议

如果你的目标是：
- **学习解释器实现** - 先完成高优先级功能
- **实用的脚本语言** - 异常处理和with语句更重要
- **完整的Python实现** - 需要全部实现

对于大多数用例，当前77%的完成度已经足够使用了！

建议先完成：
1. 装饰器系统
2. Set类型
3. 字典推导式
4. __contains__魔术方法

这些功能更实用，实现也更简单（总共约10-15小时）。

完成这些后，如果还有时间和精力，再考虑实现异常处理和with语句。

生成器和async/await可以作为长期目标，它们需要大量的时间和精力投入。
