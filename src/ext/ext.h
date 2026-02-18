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

// 扩展API
void ms_register_extension(ms_vm_t* vm, ms_extension_t* ext);
ms_value_t ms_call_extension_function(ms_vm_t* vm, const char* module, const char* func, int argc, ms_value_t* args);

#endif // EXT_H
