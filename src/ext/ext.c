#include "ext.h"
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
    #define LOAD_LIBRARY(path) LoadLibraryA(path)
    #define GET_PROC_ADDRESS(handle, name) GetProcAddress((HMODULE)handle, name)
    #define FREE_LIBRARY(handle) FreeLibrary((HMODULE)handle)
    #define LIB_EXTENSION ".dll"
#else
    #include <dlfcn.h>
    #define LOAD_LIBRARY(path) dlopen(path, RTLD_LAZY)
    #define GET_PROC_ADDRESS(handle, name) dlsym(handle, name)
    #define FREE_LIBRARY(handle) dlclose(handle)
    #ifdef __APPLE__
        #define LIB_EXTENSION ".dylib"
    #else
        #define LIB_EXTENSION ".so"
    #endif
#endif

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

// 动态库加载函数
ms_dynamic_extension_t* ms_load_extension_library(const char* lib_path) {
    ms_dynamic_extension_t* dyn_ext = malloc(sizeof(ms_dynamic_extension_t));
    if (!dyn_ext) return NULL;
    
    // 加载动态库
    void* handle = LOAD_LIBRARY(lib_path);
    if (!handle) {
        free(dyn_ext);
        return NULL;
    }
    
    // 查找扩展创建函数 (约定: ms_<module_name>_extension_create)
    // 例如: ms_myext_extension_create
    typedef ms_extension_t* (*ext_create_fn_t)(void);
    
    // 尝试通用的创建函数名
    ext_create_fn_t create_fn = (ext_create_fn_t)GET_PROC_ADDRESS(handle, "ms_extension_create");
    
    if (!create_fn) {
        FREE_LIBRARY(handle);
        free(dyn_ext);
        return NULL;
    }
    
    ms_extension_t* ext = create_fn();
    if (!ext) {
        FREE_LIBRARY(handle);
        free(dyn_ext);
        return NULL;
    }
    
    dyn_ext->name = ext->name;
    dyn_ext->handle = handle;
    dyn_ext->ext = ext;
    
    return dyn_ext;
}

void ms_unload_extension_library(ms_dynamic_extension_t* dyn_ext) {
    if (!dyn_ext) return;
    
    // 查找销毁函数
    typedef void (*ext_destroy_fn_t)(ms_extension_t*);
    ext_destroy_fn_t destroy_fn = (ext_destroy_fn_t)GET_PROC_ADDRESS(dyn_ext->handle, "ms_extension_destroy");
    
    if (destroy_fn && dyn_ext->ext) {
        destroy_fn(dyn_ext->ext);
    }
    
    if (dyn_ext->handle) {
        FREE_LIBRARY(dyn_ext->handle);
    }
    
    free(dyn_ext);
}

ms_extension_t* ms_get_extension_from_library(ms_dynamic_extension_t* dyn_ext) {
    if (!dyn_ext) return NULL;
    return dyn_ext->ext;
}
