#ifndef JIT_H
#define JIT_H

#include "../vm/vm.h"
#include <stdint.h>

// JIT编译器状态
typedef enum {
    JIT_STATE_DISABLED,
    JIT_STATE_PROFILING,
    JIT_STATE_COMPILING,
    JIT_STATE_COMPILED
} ms_jit_state_t;

// 热点信息
typedef struct {
    uint8_t* bytecode_start;
    int bytecode_length;
    int call_count;
    void* native_code;
    size_t native_size;
    ms_jit_state_t state;
} ms_hotspot_t;

// JIT编译器
typedef struct {
    ms_hotspot_t* hotspots;
    int hotspot_count;
    int hotspot_capacity;
    bool enabled;
    int threshold;
} ms_jit_compiler_t;

// JIT API
void ms_jit_init(ms_jit_compiler_t* jit);
void ms_jit_free(ms_jit_compiler_t* jit);
void ms_jit_profile(ms_jit_compiler_t* jit, uint8_t* ip);
bool ms_jit_compile_hotspot(ms_jit_compiler_t* jit, ms_hotspot_t* hotspot);
void* ms_jit_get_native_code(ms_jit_compiler_t* jit, uint8_t* ip);

#endif // JIT_H