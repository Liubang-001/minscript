#include "math_ext.h"
#include <stdlib.h>
#include <math.h>

// Math extension functions
static ms_value_t math_sqrt(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc < 1) return ms_value_nil();
    
    double value = 0;
    if (ms_value_is_int(args[0])) {
        value = (double)ms_value_as_int(args[0]);
    } else if (ms_value_is_float(args[0])) {
        value = ms_value_as_float(args[0]);
    } else {
        return ms_value_nil();
    }
    
    if (value < 0) return ms_value_nil();
    return ms_value_float(sqrt(value));
}

static ms_value_t math_pow(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc < 2) return ms_value_nil();
    
    double base = 0, exp = 0;
    
    if (ms_value_is_int(args[0])) {
        base = (double)ms_value_as_int(args[0]);
    } else if (ms_value_is_float(args[0])) {
        base = ms_value_as_float(args[0]);
    } else {
        return ms_value_nil();
    }
    
    if (ms_value_is_int(args[1])) {
        exp = (double)ms_value_as_int(args[1]);
    } else if (ms_value_is_float(args[1])) {
        exp = ms_value_as_float(args[1]);
    } else {
        return ms_value_nil();
    }
    
    return ms_value_float(pow(base, exp));
}

static ms_value_t math_sin(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc < 1) return ms_value_nil();
    
    double value = 0;
    if (ms_value_is_int(args[0])) {
        value = (double)ms_value_as_int(args[0]);
    } else if (ms_value_is_float(args[0])) {
        value = ms_value_as_float(args[0]);
    } else {
        return ms_value_nil();
    }
    
    return ms_value_float(sin(value));
}

static ms_value_t math_cos(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc < 1) return ms_value_nil();
    
    double value = 0;
    if (ms_value_is_int(args[0])) {
        value = (double)ms_value_as_int(args[0]);
    } else if (ms_value_is_float(args[0])) {
        value = ms_value_as_float(args[0]);
    } else {
        return ms_value_nil();
    }
    
    return ms_value_float(cos(value));
}

static ms_value_t math_abs(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc < 1) return ms_value_nil();
    
    if (ms_value_is_int(args[0])) {
        int64_t value = ms_value_as_int(args[0]);
        return ms_value_int(value < 0 ? -value : value);
    } else if (ms_value_is_float(args[0])) {
        double value = ms_value_as_float(args[0]);
        return ms_value_float(value < 0 ? -value : value);
    }
    
    return ms_value_nil();
}

static ms_value_t math_floor(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc < 1) return ms_value_nil();
    
    double value = 0;
    if (ms_value_is_int(args[0])) {
        return args[0];  // Already an integer
    } else if (ms_value_is_float(args[0])) {
        value = ms_value_as_float(args[0]);
    } else {
        return ms_value_nil();
    }
    
    return ms_value_int((int64_t)floor(value));
}

static ms_value_t math_ceil(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc < 1) return ms_value_nil();
    
    double value = 0;
    if (ms_value_is_int(args[0])) {
        return args[0];  // Already an integer
    } else if (ms_value_is_float(args[0])) {
        value = ms_value_as_float(args[0]);
    } else {
        return ms_value_nil();
    }
    
    return ms_value_int((int64_t)ceil(value));
}

// Create math extension
ms_extension_t* ms_math_extension_create(void) {
    ms_extension_t* ext = malloc(sizeof(ms_extension_t));
    ext->name = "math";
    ext->function_count = 7;
    
    ext->functions[0].name = "sqrt";
    ext->functions[0].func = math_sqrt;
    
    ext->functions[1].name = "pow";
    ext->functions[1].func = math_pow;
    
    ext->functions[2].name = "sin";
    ext->functions[2].func = math_sin;
    
    ext->functions[3].name = "cos";
    ext->functions[3].func = math_cos;
    
    ext->functions[4].name = "abs";
    ext->functions[4].func = math_abs;
    
    ext->functions[5].name = "floor";
    ext->functions[5].func = math_floor;
    
    ext->functions[6].name = "ceil";
    ext->functions[6].func = math_ceil;
    
    return ext;
}

// Destroy math extension
void ms_math_extension_destroy(ms_extension_t* ext) {
    if (ext) {
        free(ext);
    }
}
