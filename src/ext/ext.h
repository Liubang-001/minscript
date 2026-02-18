#ifndef EXT_H
#define EXT_H

#include "../vm/vm.h"
#include <stdint.h>

// 扩展函数类型
typedef ms_value_t (*ms_ext_fn_t)(ms_vm_t* vm, int argc, ms_value_t* args);

// 扩展模块结构
typedef struct {
    const char* name;
    struct {
        const char* name;
        ms_ext_fn_t func;
    } functions[64];
    int function_count;
} ms_extension_t;

// 动态库扩展结构
typedef struct {
    const char* name;
    void* handle;  // 动态库句柄
    ms_extension_t* ext;
} ms_dynamic_extension_t;

// 扩展API
void ms_register_extension(ms_vm_t* vm, ms_extension_t* ext);
ms_value_t ms_call_extension_function(ms_vm_t* vm, const char* module, const char* func, int argc, ms_value_t* args);

// 动态库加载API
ms_dynamic_extension_t* ms_load_extension_library(const char* lib_path);
void ms_unload_extension_library(ms_dynamic_extension_t* dyn_ext);
ms_extension_t* ms_get_extension_from_library(ms_dynamic_extension_t* dyn_ext);

#endif // EXT_H
