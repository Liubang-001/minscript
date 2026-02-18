#include "miniscript.h"
#include "ext/ext.h"
#include "ext/http.h"
#include "ext/math_ext.h"
#include "ext/string_ext.h"
#include "builtins/builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void repl(ms_vm_t* vm) {
    char line[1024];
    
    for (;;) {
        printf("> ");
        
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        
        ms_result_t result = ms_vm_exec_string(vm, line);
        if (result == MS_RESULT_COMPILE_ERROR) {
            printf("Compile error\n");
        } else if (result == MS_RESULT_RUNTIME_ERROR) {
            printf("Runtime error: %s\n", ms_vm_get_error(vm));
            ms_vm_clear_error(vm);
        }
    }
}

static void run_file(ms_vm_t* vm, const char* path) {
    ms_result_t result = ms_vm_exec_file(vm, path);
    
    if (result == MS_RESULT_COMPILE_ERROR) {
        fprintf(stderr, "Compile error\n");
        exit(65);
    }
    if (result == MS_RESULT_RUNTIME_ERROR) {
        fprintf(stderr, "Runtime error: %s\n", ms_vm_get_error(vm));
        exit(70);
    }
}

int main(int argc, const char* argv[]) {
    ms_vm_t* vm = ms_vm_new();
    
    // 注册所有内置函数
    ms_register_builtins(vm);
    
    // 注册扩展
    ms_extension_t* http_ext = ms_http_extension_create();
    ms_register_extension(vm, http_ext);
    
    ms_extension_t* math_ext = ms_math_extension_create();
    ms_register_extension(vm, math_ext);
    
    ms_extension_t* string_ext = ms_string_extension_create();
    ms_register_extension(vm, string_ext);
    
    if (argc == 1) {
        printf("MiniScript v%d.%d.%d (Python 3 syntax)\n", 
               MS_VERSION_MAJOR, MS_VERSION_MINOR, MS_VERSION_PATCH);
        repl(vm);
    } else if (argc == 2) {
        run_file(vm, argv[1]);
    } else {
        fprintf(stderr, "Usage: miniscript [path]\n");
        exit(64);
    }
    
    ms_http_extension_destroy(http_ext);
    ms_math_extension_destroy(math_ext);
    ms_string_extension_destroy(string_ext);
    ms_vm_free(vm);
    return 0;
}