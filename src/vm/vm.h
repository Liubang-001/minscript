#ifndef VM_H
#define VM_H

#include "miniscript.h"
#include "../lexer/lexer.h"
#include <stdint.h>

// 字节码块
typedef struct {
    int count;
    int capacity;
    uint8_t* code;
    int* lines;
    ms_value_t* constants;
    int constant_count;
    int constant_capacity;
} ms_chunk_t;

// 函数对象
typedef struct {
    ms_chunk_t* chunk;
    int arity;  // 参数个数
    char* name;
    int default_count;  // 有默认值的参数个数
    ms_value_t* defaults;  // 默认值数组
} ms_function_t;

// 字节码指令
typedef enum {
    OP_CONSTANT,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_POP,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_GET_GLOBAL,
    OP_DEFINE_GLOBAL,
    OP_SET_GLOBAL,
    OP_GET_UPVALUE,
    OP_SET_UPVALUE,
    OP_GET_PROPERTY,
    OP_SET_PROPERTY,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_GREATER_EQUAL,  // >=
    OP_LESS_EQUAL,     // <=
    OP_IN,             // in 运算符
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_FLOOR_DIVIDE,  // 整除 //
    OP_POWER,         // 幂运算 **
    OP_MODULO,
    OP_NOT,
    OP_NEGATE,
    OP_PRINT,
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_JUMP_IF_TRUE,
    OP_LOOP,
    OP_CALL,
    OP_CALL_METHOD,
    OP_CALL_DECORATOR,  // 调用装饰器
    OP_CALL_ENTER,      // 调用 __enter__
    OP_CALL_EXIT,       // 调用 __exit__
    OP_FUNCTION,
    OP_CLOSURE,
    OP_CLOSE_UPVALUE,
    OP_RETURN,
    OP_LOAD_MODULE,
    OP_CLASS,
    OP_INHERIT,
    OP_METHOD,
    OP_BUILD_LIST,
    OP_BUILD_DICT,
    OP_BUILD_TUPLE,
    OP_BUILD_SET,
    OP_SET_ADD,
    OP_INDEX_GET,
    OP_INDEX_SET,
    OP_SLICE_GET,
    OP_FOR_ITER,
    OP_FOR_ITER_LOCAL,
    OP_FOR_END,
    OP_TERNARY,  // 三元表达式
    OP_DUP,      // 复制栈顶值
    OP_SWAP,     // 交换栈顶两个值
    OP_BUILD_LIST_COMP,  // 列表推导式
    OP_LIST_APPEND,  // 向列表添加元素
    OP_BUILD_DICT_COMP,  // 字典推导式
    OP_LAMBDA,   // lambda 表达式
    OP_ASSERT,   // assert 语句
    OP_DELETE,   // del 语句
    OP_TRY_BEGIN,      // 标记try块开始
    OP_TRY_END,        // 标记try块结束
    OP_RAISE,          // 抛出异常
    OP_JUMP_IF_EXCEPTION,  // 如果有异常则跳转
} ms_opcode_t;

// 调用帧
typedef struct {
    uint8_t* ip;
    ms_value_t* slots;
} ms_call_frame_t;

// 异常处理器
typedef struct {
    uint8_t* handler_ip;   // Jump target for exception handler
    int stack_height;      // Stack height to restore
    int frame_index;       // Frame index when handler was pushed
} ms_exception_handler_t;

// 全局变量表
typedef struct ms_global {
    char* name;
    ms_value_t value;
    struct ms_global* next;
} ms_global_t;

// 名称表（用于编译时）
extern char* name_table_names[256];
extern int name_table_count;

// 虚拟机结构
struct ms_vm {
    ms_chunk_t* chunk;
    uint8_t* ip;
    
    ms_value_t stack[MS_MAX_STACK_SIZE];
    ms_value_t* stack_top;
    
    ms_call_frame_t frames[64];
    int frame_count;
    
    ms_global_t* globals;
    
    // Exception handling
    ms_exception_handler_t exception_handlers[64];
    int exception_handler_count;
    ms_value_t current_exception;
    bool has_exception;
    
    // For tracking module method calls
    const char* last_method_name;
    const char* last_module_name;
    
    // For dynamic extensions
    void* dynamic_extensions[32];
    int dynamic_extension_count;
    
    char error_message[256];
    bool has_error;
    
    // JIT相关
    bool jit_enabled;
    int hotspot_threshold;
};

// 字节码块操作
void ms_chunk_init(ms_chunk_t* chunk);
void ms_chunk_free(ms_chunk_t* chunk);
void ms_chunk_write(ms_chunk_t* chunk, uint8_t byte, int line);
int ms_chunk_add_constant(ms_chunk_t* chunk, ms_value_t value);

// VM操作
ms_result_t ms_vm_interpret(ms_vm_t* vm, ms_chunk_t* chunk);
void ms_vm_reset_stack(ms_vm_t* vm);

#endif // VM_H