#ifndef VM_H
#define VM_H

#include "miniscript.h"
#include "../lexer/lexer.h"
#include <stdint.h>

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
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_PRINT,
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_LOOP,
    OP_CALL,
    OP_RETURN,
} ms_opcode_t;

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

// 调用帧
typedef struct {
    uint8_t* ip;
    ms_value_t* slots;
} ms_call_frame_t;

// 全局变量表
typedef struct ms_global {
    char* name;
    ms_value_t value;
    struct ms_global* next;
} ms_global_t;

// 虚拟机结构
struct ms_vm {
    ms_chunk_t* chunk;
    uint8_t* ip;
    
    ms_value_t stack[MS_MAX_STACK_SIZE];
    ms_value_t* stack_top;
    
    ms_call_frame_t frames[64];
    int frame_count;
    
    ms_global_t* globals;
    
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