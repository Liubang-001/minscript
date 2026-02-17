#ifndef MINISCRIPT_H
#define MINISCRIPT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 版本信息
#define MS_VERSION_MAJOR 1
#define MS_VERSION_MINOR 0
#define MS_VERSION_PATCH 0

// 配置选项
#ifndef MS_MAX_STACK_SIZE
#define MS_MAX_STACK_SIZE 1024
#endif

#ifndef MS_MAX_LOCALS
#define MS_MAX_LOCALS 256
#endif

#ifndef MS_MAX_CONSTANTS
#define MS_MAX_CONSTANTS 256
#endif

// 前向声明
typedef struct ms_vm ms_vm_t;
typedef struct ms_value ms_value_t;
typedef struct ms_object ms_object_t;
typedef struct ms_native_func ms_native_func_t;

// 原生函数类型
typedef ms_value_t (*ms_native_fn_t)(ms_vm_t* vm, int argc, ms_value_t* args);

// 原生函数结构
struct ms_native_func {
    ms_native_fn_t func;
    char* name;
};

// 值类型
typedef enum {
    MS_VAL_NIL,
    MS_VAL_BOOL,
    MS_VAL_INT,
    MS_VAL_FLOAT,
    MS_VAL_STRING,
    MS_VAL_FUNCTION,
    MS_VAL_NATIVE_FUNC,
    MS_VAL_OBJECT,
    MS_VAL_LIST,
    MS_VAL_DICT,
    MS_VAL_CLASS,
    MS_VAL_INSTANCE
} ms_value_type_t;

// 值结构
struct ms_value {
    ms_value_type_t type;
    union {
        bool boolean;
        int64_t integer;
        double floating;
        char* string;
        struct ms_function* function;
        ms_native_func_t* native_func;
        ms_object_t* object;
    } as;
};

// VM结果类型
typedef enum {
    MS_RESULT_OK,
    MS_RESULT_COMPILE_ERROR,
    MS_RESULT_RUNTIME_ERROR
} ms_result_t;

// 核心API
ms_vm_t* ms_vm_new(void);
void ms_vm_free(ms_vm_t* vm);
ms_result_t ms_vm_exec_string(ms_vm_t* vm, const char* source);
ms_result_t ms_vm_exec_file(ms_vm_t* vm, const char* filename);

// 值操作
ms_value_t ms_value_nil(void);
ms_value_t ms_value_bool(bool value);
ms_value_t ms_value_int(int64_t value);
ms_value_t ms_value_float(double value);
ms_value_t ms_value_string(const char* value);
ms_value_t ms_value_native_func(ms_native_fn_t func);

bool ms_value_is_nil(ms_value_t value);
bool ms_value_is_bool(ms_value_t value);
bool ms_value_is_int(ms_value_t value);
bool ms_value_is_float(ms_value_t value);
bool ms_value_is_string(ms_value_t value);
bool ms_value_is_function(ms_value_t value);

bool ms_value_as_bool(ms_value_t value);
int64_t ms_value_as_int(ms_value_t value);
double ms_value_as_float(ms_value_t value);
const char* ms_value_as_string(ms_value_t value);

// 扩展API
void ms_vm_register_function(ms_vm_t* vm, const char* name, ms_native_fn_t func);
void ms_vm_set_global(ms_vm_t* vm, const char* name, ms_value_t value);
ms_value_t ms_vm_get_global(ms_vm_t* vm, const char* name);

// 栈操作
void ms_vm_push(ms_vm_t* vm, ms_value_t value);
ms_value_t ms_vm_pop(ms_vm_t* vm);
ms_value_t ms_vm_peek(ms_vm_t* vm, int distance);

// 错误处理
const char* ms_vm_get_error(ms_vm_t* vm);
void ms_vm_clear_error(ms_vm_t* vm);

#ifdef __cplusplus
}
#endif

#endif // MINISCRIPT_H