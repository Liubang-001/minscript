#include "example_ext.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// 示例函数1: greet
static ms_value_t example_greet(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    
    if (argc < 1) {
        return ms_value_string("Hello, World!");
    }
    
    if (ms_value_is_string(args[0])) {
        const char* name = ms_value_as_string(args[0]);
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "Hello, %s!", name);
        char* result = malloc(strlen(buffer) + 1);
        strcpy(result, buffer);
        return ms_value_string(result);
    }
    
    return ms_value_string("Hello!");
}

// 示例函数2: add
static ms_value_t example_add(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    
    if (argc < 2) {
        return ms_value_int(0);
    }
    
    if (ms_value_is_int(args[0]) && ms_value_is_int(args[1])) {
        return ms_value_int(ms_value_as_int(args[0]) + ms_value_as_int(args[1]));
    }
    
    return ms_value_int(0);
}

// 示例函数3: multiply
static ms_value_t example_multiply(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    
    if (argc < 2) {
        return ms_value_int(0);
    }
    
    if (ms_value_is_int(args[0]) && ms_value_is_int(args[1])) {
        return ms_value_int(ms_value_as_int(args[0]) * ms_value_as_int(args[1]));
    }
    
    return ms_value_int(0);
}

// 创建扩展
ms_extension_t* ms_extension_create(void) {
    ms_extension_t* ext = malloc(sizeof(ms_extension_t));
    if (!ext) return NULL;
    
    ext->name = "example";
    ext->function_count = 3;
    
    // 注册函数
    ext->functions[0].name = "greet";
    ext->functions[0].func = example_greet;
    
    ext->functions[1].name = "add";
    ext->functions[1].func = example_add;
    
    ext->functions[2].name = "multiply";
    ext->functions[2].func = example_multiply;
    
    return ext;
}

// 销毁扩展
void ms_extension_destroy(ms_extension_t* ext) {
    if (ext) {
        free(ext);
    }
}
