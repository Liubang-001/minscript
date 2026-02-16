#include "miniscript.h"
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

// 内置函数示例
static ms_value_t builtin_len(ms_vm_t* vm, int argc, ms_value_t* args) {
    if (argc != 1) {
        return ms_value_nil();
    }
    
    if (ms_value_is_string(args[0])) {
        return ms_value_int(strlen(ms_value_as_string(args[0])));
    }
    
    return ms_value_nil();
}

int main(int argc, const char* argv[]) {
    ms_vm_t* vm = ms_vm_new();
    
    // 注册内置函数
    ms_vm_register_function(vm, "len", builtin_len);
    
    if (argc == 1) {
        printf("MiniScript v%d.%d.%d\n", 
               MS_VERSION_MAJOR, MS_VERSION_MINOR, MS_VERSION_PATCH);
        repl(vm);
    } else if (argc == 2) {
        run_file(vm, argv[1]);
    } else {
        fprintf(stderr, "Usage: miniscript [path]\n");
        exit(64);
    }
    
    ms_vm_free(vm);
    return 0;
}