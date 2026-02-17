#include "jit.h"
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <unistd.h>
#endif

// 跨平台内存分配函数
static void* allocate_executable_memory(size_t size) {
#ifdef _WIN32
    return VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#else
    return mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC,
                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif
}

static void free_executable_memory(void* ptr, size_t size) {
#ifdef _WIN32
    VirtualFree(ptr, 0, MEM_RELEASE);
#else
    munmap(ptr, size);
#endif
}

static bool protect_executable_memory(void* ptr, size_t size) {
#ifdef _WIN32
    DWORD old_protect;
    return VirtualProtect(ptr, size, PAGE_EXECUTE_READ, &old_protect) != 0;
#else
    return mprotect(ptr, size, PROT_READ | PROT_EXEC) == 0;
#endif
}

// 简化的x86-64机器码生成
static void emit_mov_rax_imm(uint8_t** code, int64_t value) {
    // mov rax, imm64
    *(*code)++ = 0x48;
    *(*code)++ = 0xb8;
    memcpy(*code, &value, 8);
    *code += 8;
}

static void emit_add_rax_rbx(uint8_t** code) {
    // add rax, rbx
    *(*code)++ = 0x48;
    *(*code)++ = 0x01;
    *(*code)++ = 0xd8;
}

static void emit_ret(uint8_t** code) {
    // ret
    *(*code)++ = 0xc3;
}

void ms_jit_init(ms_jit_compiler_t* jit) {
    jit->hotspots = NULL;
    jit->hotspot_count = 0;
    jit->hotspot_capacity = 0;
    jit->enabled = true;
    jit->threshold = 100;
}

void ms_jit_free(ms_jit_compiler_t* jit) {
    for (int i = 0; i < jit->hotspot_count; i++) {
        if (jit->hotspots[i].native_code) {
            free_executable_memory(jit->hotspots[i].native_code, jit->hotspots[i].native_size);
        }
    }
    free(jit->hotspots);
}

void ms_jit_profile(ms_jit_compiler_t* jit, uint8_t* ip) {
    if (!jit->enabled) return;
    
    // 查找现有热点
    for (int i = 0; i < jit->hotspot_count; i++) {
        ms_hotspot_t* hotspot = &jit->hotspots[i];
        if (ip >= hotspot->bytecode_start && 
            ip < hotspot->bytecode_start + hotspot->bytecode_length) {
            hotspot->call_count++;
            
            // 达到阈值，触发编译
            if (hotspot->call_count >= jit->threshold && 
                hotspot->state == JIT_STATE_PROFILING) {
                hotspot->state = JIT_STATE_COMPILING;
                if (ms_jit_compile_hotspot(jit, hotspot)) {
                    hotspot->state = JIT_STATE_COMPILED;
                } else {
                    hotspot->state = JIT_STATE_DISABLED;
                }
            }
            return;
        }
    }
    
    // 创建新热点
    if (jit->hotspot_count >= jit->hotspot_capacity) {
        int old_capacity = jit->hotspot_capacity;
        jit->hotspot_capacity = old_capacity < 8 ? 8 : old_capacity * 2;
        jit->hotspots = realloc(jit->hotspots, 
                               sizeof(ms_hotspot_t) * jit->hotspot_capacity);
    }
    
    ms_hotspot_t* hotspot = &jit->hotspots[jit->hotspot_count++];
    hotspot->bytecode_start = ip;
    hotspot->bytecode_length = 16; // 简化：固定长度
    hotspot->call_count = 1;
    hotspot->native_code = NULL;
    hotspot->native_size = 0;
    hotspot->state = JIT_STATE_PROFILING;
}

bool ms_jit_compile_hotspot(ms_jit_compiler_t* jit, ms_hotspot_t* hotspot) {
    // 分配可执行内存
    size_t code_size = 1024; // 简化：固定大小
    void* code = allocate_executable_memory(code_size);
    
    if (code == NULL) {
        return false;
    }
    
    uint8_t* code_ptr = (uint8_t*)code;
    
    // 简化的编译：只处理基本算术运算
    uint8_t* ip = hotspot->bytecode_start;
    for (int i = 0; i < hotspot->bytecode_length && 
         code_ptr < (uint8_t*)code + code_size - 16; i++) {
        
        switch (*ip) {
            case OP_CONSTANT:
                // 简化：假设是整数常量
                emit_mov_rax_imm(&code_ptr, 42);
                ip += 2; // 跳过操作码和常量索引
                break;
            case OP_ADD:
                emit_add_rax_rbx(&code_ptr);
                ip++;
                break;
            case OP_RETURN:
                emit_ret(&code_ptr);
                ip++;
                goto compile_done;
            default:
                ip++;
                break;
        }
    }
    
    emit_ret(&code_ptr);
    
compile_done:
    hotspot->native_code = code;
    hotspot->native_size = code_size;
    
    // 设置内存为只读+可执行
    if (!protect_executable_memory(code, code_size)) {
        free_executable_memory(code, code_size);
        return false;
    }
    
    return true;
}

void* ms_jit_get_native_code(ms_jit_compiler_t* jit, uint8_t* ip) {
    for (int i = 0; i < jit->hotspot_count; i++) {
        ms_hotspot_t* hotspot = &jit->hotspots[i];
        if (hotspot->state == JIT_STATE_COMPILED &&
            ip >= hotspot->bytecode_start && 
            ip < hotspot->bytecode_start + hotspot->bytecode_length) {
            return hotspot->native_code;
        }
    }
    return NULL;
}
