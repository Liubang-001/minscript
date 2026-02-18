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
typedef struct ms_object ms_object_t;
typedef struct ms_native_func ms_native_func_t;
typedef struct ms_value ms_value_t;

// 值类型
typedef enum {
    MS_VAL_NIL,
    MS_VAL_BOOL,
    MS_VAL_INT,
    MS_VAL_FLOAT,
    MS_VAL_STRING,
    MS_VAL_FUNCTION,
    MS_VAL_NATIVE_FUNC,
    MS_VAL_MODULE,
    MS_VAL_OBJECT,
    MS_VAL_LIST,
    MS_VAL_DICT,
    MS_VAL_TUPLE,
    MS_VAL_CLASS,
    MS_VAL_INSTANCE
} ms_value_type_t;

// Forward declare collection types
typedef struct ms_list_s ms_list_t;
typedef struct ms_dict_s ms_dict_t;
typedef struct ms_tuple_s ms_tuple_t;

// 原生函数类型 (must be after ms_value_t forward declaration)
typedef ms_value_t (*ms_native_fn_t)(ms_vm_t* vm, int argc, ms_value_t* args);

// 原生函数结构
struct ms_native_func {
    ms_native_fn_t func;
    char* name;
};

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
        void* module;  // Extension module pointer
        ms_object_t* object;
        ms_list_t* list;
        ms_dict_t* dict;
        ms_tuple_t* tuple;
    } as;
};

// List 结构
struct ms_list_s {
    ms_value_t* elements;
    int count;
    int capacity;
};

// Dict 键值对
typedef struct {
    char* key;
    ms_value_t value;
} ms_dict_entry_t;

// Dict 结构
struct ms_dict_s {
    ms_dict_entry_t* entries;
    int count;
    int capacity;
};

// Tuple 结构（不可变列表）
struct ms_tuple_s {
    ms_value_t* elements;
    int count;
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
ms_value_t ms_value_list(ms_list_t* list);
ms_value_t ms_value_dict(ms_dict_t* dict);
ms_value_t ms_value_tuple(ms_tuple_t* tuple);

bool ms_value_is_nil(ms_value_t value);
bool ms_value_is_bool(ms_value_t value);
bool ms_value_is_int(ms_value_t value);
bool ms_value_is_float(ms_value_t value);
bool ms_value_is_string(ms_value_t value);
bool ms_value_is_function(ms_value_t value);
bool ms_value_is_list(ms_value_t value);
bool ms_value_is_dict(ms_value_t value);
bool ms_value_is_tuple(ms_value_t value);

bool ms_value_as_bool(ms_value_t value);
int64_t ms_value_as_int(ms_value_t value);
double ms_value_as_float(ms_value_t value);
const char* ms_value_as_string(ms_value_t value);
ms_list_t* ms_value_as_list(ms_value_t value);
ms_dict_t* ms_value_as_dict(ms_value_t value);
ms_tuple_t* ms_value_as_tuple(ms_value_t value);

// Collection operations
ms_list_t* ms_list_new(void);
void ms_list_free(ms_list_t* list);
void ms_list_append(ms_list_t* list, ms_value_t value);
ms_value_t ms_list_get(ms_list_t* list, int index);
void ms_list_set(ms_list_t* list, int index, ms_value_t value);
int ms_list_len(ms_list_t* list);

ms_dict_t* ms_dict_new(void);
void ms_dict_free(ms_dict_t* dict);
void ms_dict_set(ms_dict_t* dict, const char* key, ms_value_t value);
ms_value_t ms_dict_get(ms_dict_t* dict, const char* key);
bool ms_dict_has(ms_dict_t* dict, const char* key);
int ms_dict_len(ms_dict_t* dict);

ms_tuple_t* ms_tuple_new(int count);
void ms_tuple_free(ms_tuple_t* tuple);
ms_value_t ms_tuple_get(ms_tuple_t* tuple, int index);
int ms_tuple_len(ms_tuple_t* tuple);

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