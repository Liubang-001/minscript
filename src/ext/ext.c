#include "ext.h"
#include <stdlib.h>
#include <string.h>

// 扩展注册表
typedef struct {
    ms_extension_t* extensions[32];
    int count;
} ms_extension_registry_t;

static ms_extension_registry_t registry = {0};

void ms_register_extension(ms_vm_t* vm, ms_extension_t* ext) {
    if (registry.count >= 32) {
        return;
    }
    registry.extensions[registry.count++] = ext;
}

ms_value_t ms_call_extension_function(ms_vm_t* vm, const char* module, const char* func, int argc, ms_value_t* args) {
    // 查找模块
    for (int i = 0; i < registry.count; i++) {
        if (strcmp(registry.extensions[i]->name, module) == 0) {
            // 查找函数
            for (int j = 0; j < registry.extensions[i]->function_count; j++) {
                if (strcmp(registry.extensions[i]->functions[j].name, func) == 0) {
                    return registry.extensions[i]->functions[j].func(vm, argc, args);
                }
            }
        }
    }
    
    return ms_value_nil();
}
