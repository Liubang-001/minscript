#ifndef COMPILER_H
#define COMPILER_H

#include "../vm/vm.h"
#include "../lexer/lexer.h"

// 编译器状态
typedef struct {
    ms_lexer_t* lexer;
    ms_token_t current;
    ms_token_t previous;
    bool had_error;
    bool panic_mode;
} ms_compiler_t;

// 局部变量
typedef struct {
    ms_token_t name;
    int depth;
    bool is_captured;
} ms_local_t;

// 作用域深度
typedef struct ms_compiler_scope {
    struct ms_compiler_scope* enclosing;
    ms_chunk_t* chunk;
    
    ms_local_t locals[256];
    int local_count;
    int scope_depth;
    
    // 函数信息
    bool is_function;
    int arity;
} ms_compiler_scope_t;

// 编译API
bool ms_compile_enhanced(const char* source, ms_chunk_t* chunk);

#endif // COMPILER_H