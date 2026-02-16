#include "vm.h"
#include <stdlib.h>

void ms_chunk_init(ms_chunk_t* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
    chunk->constants = NULL;
    chunk->constant_count = 0;
    chunk->constant_capacity = 0;
}

void ms_chunk_free(ms_chunk_t* chunk) {
    free(chunk->code);
    free(chunk->lines);
    free(chunk->constants);
    ms_chunk_init(chunk);
}

void ms_chunk_write(ms_chunk_t* chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int old_capacity = chunk->capacity;
        chunk->capacity = old_capacity < 8 ? 8 : old_capacity * 2;
        chunk->code = realloc(chunk->code, sizeof(uint8_t) * chunk->capacity);
        chunk->lines = realloc(chunk->lines, sizeof(int) * chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

int ms_chunk_add_constant(ms_chunk_t* chunk, ms_value_t value) {
    if (chunk->constant_capacity < chunk->constant_count + 1) {
        int old_capacity = chunk->constant_capacity;
        chunk->constant_capacity = old_capacity < 8 ? 8 : old_capacity * 2;
        chunk->constants = realloc(chunk->constants, 
                                 sizeof(ms_value_t) * chunk->constant_capacity);
    }

    chunk->constants[chunk->constant_count] = value;
    return chunk->constant_count++;
}