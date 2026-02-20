#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 外部名称表声明
extern char* name_table_names[256];
extern int name_table_count;

// 前向声明
static uint8_t add_name(const char* name, int length);
static void expression(ms_parser_t* parser);
static void statement(ms_parser_t* parser);
static void declaration(ms_parser_t* parser);
static ms_parse_rule_t* get_rule(ms_token_type_t type);
static void parse_precedence(ms_parser_t* parser, ms_precedence_t precedence);
static int emit_jump(ms_parser_t* parser, uint8_t instruction);
static void patch_jump(ms_parser_t* parser, int offset);
static void emit_loop(ms_parser_t* parser, int loop_start);
static void with_statement(ms_parser_t* parser);
static void match_statement(ms_parser_t* parser);
static void string(ms_parser_t* parser);
static void import_statement(ms_parser_t* parser);

static ms_chunk_t* current_chunk(ms_parser_t* parser) {
    return parser->compiling_chunk;
}

static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

static void error_at(ms_parser_t* parser, ms_token_t* token, const char* message) {
    if (parser->panic_mode) return;
    parser->panic_mode = true;
    
    fprintf(stderr, "[line %d] Error", token->line);
    
    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
        // Nothing.
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }
    
    fprintf(stderr, ": %s\n", message);
    parser->had_error = true;
}

static void error(ms_parser_t* parser, const char* message) {
    error_at(parser, &parser->previous, message);
}

static void error_at_current(ms_parser_t* parser, const char* message) {
    error_at(parser, &parser->current, message);
}

static void advance(ms_parser_t* parser) {
    parser->previous = parser->current;
    
    for (;;) {
        parser->current = ms_lexer_scan_token(parser->lexer);
        if (parser->current.type != TOKEN_ERROR) break;
        
        error_at_current(parser, parser->current.start);
    }
}

static void consume(ms_parser_t* parser, ms_token_type_t type, const char* message) {
    if (parser->current.type == type) {
        advance(parser);
        return;
    }
    
    error_at_current(parser, message);
}

static bool check(ms_parser_t* parser, ms_token_type_t type) {
    return parser->current.type == type;
}

static bool match(ms_parser_t* parser, ms_token_type_t type) {
    if (!check(parser, type)) return false;
    advance(parser);
    return true;
}

static void emit_byte(ms_parser_t* parser, uint8_t byte) {
    ms_chunk_write(current_chunk(parser), byte, parser->previous.line);
}

static void emit_bytes(ms_parser_t* parser, uint8_t byte1, uint8_t byte2) {
    emit_byte(parser, byte1);
    emit_byte(parser, byte2);
}

static void emit_return(ms_parser_t* parser) {
    emit_byte(parser, OP_RETURN);
}

static uint8_t make_constant(ms_parser_t* parser, ms_value_t value) {
    int constant = ms_chunk_add_constant(current_chunk(parser), value);
    if (constant > UINT8_MAX) {
        error(parser, "Too many constants in one chunk.");
        return 0;
    }
    
    return (uint8_t)constant;
}

static void emit_constant(ms_parser_t* parser, ms_value_t value) {
    emit_bytes(parser, OP_CONSTANT, make_constant(parser, value));
}

static ms_chunk_t* create_function_chunk(ms_parser_t* parser) {
    if (parser->function_chunk_count >= 256) {
        return NULL;
    }
    
    ms_chunk_t* chunk = malloc(sizeof(ms_chunk_t));
    ms_chunk_init(chunk);
    parser->function_chunks[parser->function_chunk_count++] = chunk;
    return chunk;
}

static void end_compiler(ms_parser_t* parser) {
    emit_return(parser);
}

// 前向声明
static void expression(ms_parser_t* parser);
static void statement(ms_parser_t* parser);
static void declaration(ms_parser_t* parser);
static ms_parse_rule_t* get_rule(ms_token_type_t type);
static void parse_precedence(ms_parser_t* parser, ms_precedence_t precedence);
static int emit_jump(ms_parser_t* parser, uint8_t instruction);
static void patch_jump(ms_parser_t* parser, int offset);
static void emit_loop(ms_parser_t* parser, int loop_start);

// 局部变量管理
typedef struct {
    ms_token_t name;
    int depth;
} ms_local_t;

static ms_local_t locals[256];
static int local_count = 0;
static int scope_depth = 0;

// 循环控制
static int loop_depth = 0;  // 当前循环嵌套深度
static int break_jumps[256];  // break 跳转位置列表
static int break_count = 0;
static int continue_jumps[256];  // continue 跳转位置列表
static int continue_count = 0;

// 列表推导式计数器（用于生成唯一的临时变量名）
static int listcomp_counter = 0;

static void begin_scope() {
    scope_depth++;
}

static void skip_newlines(ms_parser_t* parser) {
    while (match(parser, TOKEN_NEWLINE)) {
        // Skip empty lines
    }
}

static void end_scope(ms_parser_t* parser) {
    scope_depth--;
    
    while (local_count > 0 && locals[local_count - 1].depth > scope_depth) {
        emit_byte(parser, OP_POP);
        local_count--;
    }
}

// End scope but keep the top value on stack
// This is used for list comprehensions where we need to return a value
static void end_scope_keep_top(ms_parser_t* parser) {
    scope_depth--;
    
    // Count how many locals need to be popped
    int locals_to_pop = 0;
    int temp_local_count = local_count;
    while (temp_local_count > 0 && locals[temp_local_count - 1].depth > scope_depth) {
        locals_to_pop++;
        temp_local_count--;
    }
    
    if (locals_to_pop == 0) return;
    
    // We need to pop locals_to_pop values from the stack, but keep the top value
    // Current stack: [..., local0, local1, ..., localN-1, top_value]
    // We want: [..., top_value]
    
    // Strategy: We can't easily manipulate the middle of the stack,
    // so we'll use a temporary global variable to save the top value
    
    // Generate a unique temporary variable name
    char temp_name[32];
    snprintf(temp_name, sizeof(temp_name), "__scope_temp_%d__", listcomp_counter++);
    uint8_t temp_global = add_name(temp_name, strlen(temp_name));
    
    // Save top value to temporary global
    emit_bytes(parser, OP_DEFINE_GLOBAL, temp_global);
    // DEFINE_GLOBAL doesn't pop, so we need to pop manually
    emit_byte(parser, OP_POP);
    
    // Now pop all the locals
    for (int i = 0; i < locals_to_pop; i++) {
        emit_byte(parser, OP_POP);
        local_count--;
    }
    
    // Restore the top value from temporary global
    emit_bytes(parser, OP_GET_GLOBAL, temp_global);
}

static bool identifiers_equal(ms_token_t* a, ms_token_t* b) {
    if (a->length != b->length) return false;
    return memcmp(a->start, b->start, a->length) == 0;
}

static int resolve_local(ms_parser_t* parser, ms_token_t* name) {
    for (int i = local_count - 1; i >= 0; i--) {
        ms_local_t* local = &locals[i];
        if (identifiers_equal(name, &local->name)) {
            if (local->depth == -1) {
                error(parser, "Can't read local variable in its own initializer.");
            }
            return i;
        }
    }
    return -1;
}

static void add_local(ms_parser_t* parser, ms_token_t name) {
    if (local_count == 256) {
        error(parser, "Too many local variables in function.");
        return;
    }
    
    ms_local_t* local = &locals[local_count++];
    local->name = name;
    local->depth = -1;
}

static void mark_initialized() {
    if (scope_depth == 0) return;
    locals[local_count - 1].depth = scope_depth;
}

static uint8_t parse_variable(ms_parser_t* parser, const char* error_message) {
    consume(parser, TOKEN_IDENTIFIER, error_message);
    
    if (scope_depth > 0) {
        // 局部变量
        for (int i = local_count - 1; i >= 0; i--) {
            ms_local_t* local = &locals[i];
            if (local->depth != -1 && local->depth < scope_depth) {
                break;
            }
            if (identifiers_equal(&parser->previous, &local->name)) {
                error(parser, "Already a variable with this name in this scope.");
            }
        }
        add_local(parser, parser->previous);
        return 0;
    }
    
    return add_name(parser->previous.start, parser->previous.length);
}

static void define_variable(ms_parser_t* parser, uint8_t global) {
    if (scope_depth > 0) {
        mark_initialized();
        return;
    }
    
    emit_bytes(parser, OP_DEFINE_GLOBAL, global);
}

static void binary(ms_parser_t* parser) {
    ms_token_type_t operator_type = parser->previous.type;
    ms_parse_rule_t* rule = get_rule(operator_type);
    parse_precedence(parser, (ms_precedence_t)(rule->precedence + 1));
    
    switch (operator_type) {
        case TOKEN_BANG_EQUAL:    emit_bytes(parser, OP_EQUAL, OP_NOT); break;
        case TOKEN_EQUAL_EQUAL:   emit_byte(parser, OP_EQUAL); break;
        case TOKEN_GREATER:       emit_byte(parser, OP_GREATER); break;
        case TOKEN_GREATER_EQUAL: emit_bytes(parser, OP_LESS, OP_NOT); break;
        case TOKEN_LESS:          emit_byte(parser, OP_LESS); break;
        case TOKEN_LESS_EQUAL:    emit_bytes(parser, OP_GREATER, OP_NOT); break;
        case TOKEN_PLUS:          emit_byte(parser, OP_ADD); break;
        case TOKEN_MINUS:         emit_byte(parser, OP_SUBTRACT); break;
        case TOKEN_STAR:          emit_byte(parser, OP_MULTIPLY); break;
        case TOKEN_SLASH:         emit_byte(parser, OP_DIVIDE); break;
        case TOKEN_SLASH_SLASH:   emit_byte(parser, OP_FLOOR_DIVIDE); break;
        case TOKEN_STAR_STAR:     emit_byte(parser, OP_POWER); break;
        case TOKEN_PERCENT:       emit_byte(parser, OP_MODULO); break;
        default: return; // Unreachable.
    }
}

static void literal(ms_parser_t* parser) {
    switch (parser->previous.type) {
        case TOKEN_FALSE: emit_byte(parser, OP_FALSE); break;
        case TOKEN_NIL: emit_byte(parser, OP_NIL); break;
        case TOKEN_NONE: emit_byte(parser, OP_NIL); break;  // Python 3 None
        case TOKEN_TRUE: emit_byte(parser, OP_TRUE); break;
        default: return; // Unreachable.
    }
}

// Parse list: [1, 2, 3] or list comprehension: [expr for var in iterable]
static void list_literal(ms_parser_t* parser) {
    // Check for empty list
    if (check(parser, TOKEN_RIGHT_BRACKET)) {
        consume(parser, TOKEN_RIGHT_BRACKET, "Expect ']'.");
        emit_bytes(parser, OP_BUILD_LIST, 0);
        return;
    }
    
    // Save the start position of the expression for list comprehension
    const char* expr_start = parser->current.start;
    int expr_start_line = parser->current.line;
    
    // Try to parse as list comprehension
    // Parse first expression (but don't emit bytecode yet if it's a list comp)
    int chunk_size_before = current_chunk(parser)->count;
    expression(parser);
    
    // Check if this is a list comprehension
    if (check(parser, TOKEN_FOR)) {
        // This is a list comprehension: [expr for var in iterable]
        // We need to discard the bytecode we just generated
        current_chunk(parser)->count = chunk_size_before;
        
        // Save the end position of the expression
        const char* expr_end = parser->previous.start + parser->previous.length;
        int expr_length = expr_end - expr_start;
        
        // Now parse the for clause
        consume(parser, TOKEN_FOR, "Expect 'for'.");
        consume(parser, TOKEN_IDENTIFIER, "Expect variable name.");
        ms_token_t var_name = parser->previous;
        
        consume(parser, TOKEN_IN, "Expect 'in' after variable.");
        
        // Create a new scope for the loop variable
        begin_scope();
        
        // Parse the iterable expression FIRST (leaves value on stack)
        expression(parser);
        
        // Store iterable in a local variable (value is already on stack from expression())
        add_local(parser, (ms_token_t){.start = "__iter__", .length = 8});
        mark_initialized();
        uint8_t iter_slot = local_count - 1;
        
        // Initialize index to 0 (push value on stack)
        emit_constant(parser, ms_value_int(0));
        add_local(parser, (ms_token_t){.start = "__index__", .length = 9});
        mark_initialized();
        uint8_t index_slot = local_count - 1;
        
        // Add loop variable as local (initialize with NIL)
        emit_byte(parser, OP_NIL);
        add_local(parser, var_name);
        mark_initialized();
        uint8_t var_slot = local_count - 1;
        
        // Create a new list - keep it on stack, NOT as a local variable
        emit_bytes(parser, OP_BUILD_LIST, 0);
        // Stack: [..., iter, index, var, list]
        // Note: list is NOT a local variable, just on the stack
        
        // Loop start
        int loop_start = current_chunk(parser)->count;
        
        // Emit FOR_ITER_LOCAL instruction
        emit_bytes(parser, OP_FOR_ITER_LOCAL, var_slot);
        emit_byte(parser, iter_slot);
        emit_byte(parser, index_slot);
        
        // Jump if no more elements
        int exit_jump = emit_jump(parser, OP_JUMP_IF_FALSE);
        emit_byte(parser, OP_POP);  // Pop the boolean result
        
        // Duplicate the list (so we can append to it)
        // Stack: [..., iter, index, var, list, list]
        emit_byte(parser, OP_DUP);
        
        // Re-parse the expression with the loop variable now in scope
        // Create a temporary lexer for the expression
        char* expr_copy = malloc(expr_length + 1);
        memcpy(expr_copy, expr_start, expr_length);
        expr_copy[expr_length] = '\0';
        
        // DEBUG: Print the expression
        // printf("DEBUG: Reparsing expression: '%s'\n", expr_copy);
        
        ms_lexer_t temp_lexer;
        ms_lexer_init(&temp_lexer, expr_copy);
        temp_lexer.line = expr_start_line;
        
        // Save current parser state
        ms_lexer_t* original_lexer = parser->lexer;
        ms_token_t original_current = parser->current;
        ms_token_t original_previous = parser->previous;
        
        // Set up parser to use temporary lexer
        parser->lexer = &temp_lexer;
        // Initialize previous to a dummy token
        parser->previous = (ms_token_t){.type = TOKEN_ERROR, .start = "", .length = 0, .line = expr_start_line};
        parser->current = ms_lexer_scan_token(&temp_lexer);
        
        // Parse the expression
        expression(parser);
        
        // Restore parser state
        parser->lexer = original_lexer;
        parser->current = original_current;
        parser->previous = original_previous;
        
        // Free the expression copy
        free(expr_copy);
        
        // Append to list
        // Stack: [..., iter, index, var, list, list, element]
        emit_byte(parser, OP_LIST_APPEND);
        // Stack: [..., iter, index, var, list, list] (LIST_APPEND pops both and pushes list back)
        emit_byte(parser, OP_POP);  // Pop the returned list
        // Stack: [..., iter, index, var, list]
        
        // Loop back
        emit_loop(parser, loop_start);
        
        // Patch exit jump
        patch_jump(parser, exit_jump);
        emit_byte(parser, OP_POP);  // Pop the boolean result
        
        // Now we need to clean up the locals and leave the list on stack
        // Current stack: [..., iter, index, var, list]
        // We want: [..., list]
        
        // Use end_scope_keep_top to pop locals but keep the list
        end_scope_keep_top(parser);
        
        // Now the result list is on top of the stack
        
        consume(parser, TOKEN_RIGHT_BRACKET, "Expect ']' after list comprehension.");
        return;
    }
    
    // Regular list: [expr, expr, ...]
    int element_count = 1;
    
    while (match(parser, TOKEN_COMMA)) {
        if (check(parser, TOKEN_RIGHT_BRACKET)) break;
        expression(parser);
        element_count++;
    }
    
    consume(parser, TOKEN_RIGHT_BRACKET, "Expect ']' after list elements.");
    emit_bytes(parser, OP_BUILD_LIST, element_count);
}

// Parse dict: {"key": value, ...}
static void dict_literal(ms_parser_t* parser) {
    int pair_count = 0;
    
    if (!check(parser, TOKEN_RIGHT_BRACE)) {
        do {
            // Parse key (must be string)
            if (parser->current.type == TOKEN_STRING) {
                advance(parser);
                string(parser);
            } else {
                error(parser, "Dictionary keys must be strings.");
                return;
            }
            
            consume(parser, TOKEN_COLON, "Expect ':' after dictionary key.");
            
            // Parse value
            expression(parser);
            pair_count++;
        } while (match(parser, TOKEN_COMMA));
    }
    
    consume(parser, TOKEN_RIGHT_BRACE, "Expect '}' after dictionary elements.");
    emit_bytes(parser, OP_BUILD_DICT, pair_count);
}

// Parse tuple: (1, 2, 3)
static void tuple_literal(ms_parser_t* parser) {
    int element_count = 0;
    
    if (!check(parser, TOKEN_RIGHT_PAREN)) {
        do {
            expression(parser);
            element_count++;
        } while (match(parser, TOKEN_COMMA));
    }
    
    consume(parser, TOKEN_RIGHT_PAREN, "Expect ')' after tuple elements.");
    emit_bytes(parser, OP_BUILD_TUPLE, element_count);
}

static void grouping(ms_parser_t* parser) {
    // Check for empty tuple ()
    if (check(parser, TOKEN_RIGHT_PAREN)) {
        consume(parser, TOKEN_RIGHT_PAREN, "Expect ')'.");
        emit_bytes(parser, OP_BUILD_TUPLE, 0);
        return;
    }
    
    expression(parser);
    
    // Check if this is a tuple (has comma)
    if (match(parser, TOKEN_COMMA)) {
        int element_count = 1;
        
        // Parse remaining elements
        if (!check(parser, TOKEN_RIGHT_PAREN)) {
            do {
                expression(parser);
                element_count++;
            } while (match(parser, TOKEN_COMMA));
        }
        
        consume(parser, TOKEN_RIGHT_PAREN, "Expect ')' after tuple elements.");
        emit_bytes(parser, OP_BUILD_TUPLE, element_count);
    } else {
        // Just a grouped expression
        consume(parser, TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
    }
}

static void call(ms_parser_t* parser) {
    uint8_t arg_count = 0;
    if (!check(parser, TOKEN_RIGHT_PAREN)) {
        do {
            expression(parser);
            if (arg_count == 255) {
                error(parser, "Can't have more than 255 arguments.");
            }
            arg_count++;
        } while (match(parser, TOKEN_COMMA));
    }
    consume(parser, TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
    
    emit_bytes(parser, OP_CALL, arg_count);
}

static void index_access(ms_parser_t* parser) {
    // Handle index access: obj[index] or slice: obj[start:stop:step]
    
    // Check if this starts with a colon (e.g., [:5])
    if (check(parser, TOKEN_COLON)) {
        // Slice with no start: [:stop:step]
        emit_byte(parser, OP_NIL);  // start = nil
        consume(parser, TOKEN_COLON, "Expect ':'.");
        
        // Parse stop (optional)
        if (!check(parser, TOKEN_COLON) && !check(parser, TOKEN_RIGHT_BRACKET)) {
            expression(parser);
        } else {
            emit_byte(parser, OP_NIL);
        }
        
        // Parse step (optional)
        if (match(parser, TOKEN_COLON)) {
            if (!check(parser, TOKEN_RIGHT_BRACKET)) {
                expression(parser);
            } else {
                emit_byte(parser, OP_NIL);
            }
        } else {
            emit_byte(parser, OP_NIL);
        }
        
        consume(parser, TOKEN_RIGHT_BRACKET, "Expect ']' after slice.");
        emit_byte(parser, OP_SLICE_GET);
        return;
    }
    
    // Parse first expression (could be index or start)
    expression(parser);
    
    // Check if it's a slice (has colon after first expression)
    if (match(parser, TOKEN_COLON)) {
        // This is a slice: [start:stop:step]
        
        // Parse stop (optional)
        if (!check(parser, TOKEN_COLON) && !check(parser, TOKEN_RIGHT_BRACKET)) {
            expression(parser);
        } else {
            emit_byte(parser, OP_NIL);
        }
        
        // Parse step (optional)
        if (match(parser, TOKEN_COLON)) {
            if (!check(parser, TOKEN_RIGHT_BRACKET)) {
                expression(parser);
            } else {
                emit_byte(parser, OP_NIL);
            }
        } else {
            emit_byte(parser, OP_NIL);
        }
        
        consume(parser, TOKEN_RIGHT_BRACKET, "Expect ']' after slice.");
        emit_byte(parser, OP_SLICE_GET);
    } else {
        // Regular index access
        consume(parser, TOKEN_RIGHT_BRACKET, "Expect ']' after index.");
        emit_byte(parser, OP_INDEX_GET);
    }
}

static void attribute(ms_parser_t* parser) {
    // Handle attribute access: obj.attr or obj.method()
    consume(parser, TOKEN_IDENTIFIER, "Expect property name after '.'.");
    
    ms_token_t attr_name = parser->previous;
    uint8_t attr_index = add_name(attr_name.start, attr_name.length);
    
    // Store the attribute name as a constant for later use
    emit_bytes(parser, OP_GET_PROPERTY, attr_index);
}

// 全局变量名表（简化实现）
static uint8_t add_name(const char* name, int length) {
    for (int i = 0; i < name_table_count; i++) {
        if (strlen(name_table_names[i]) == (size_t)length &&
            memcmp(name_table_names[i], name, length) == 0) {
            return (uint8_t)i;
        }
    }
    
    if (name_table_count >= 256) return 0;
    
    name_table_names[name_table_count] = malloc(length + 1);
    memcpy(name_table_names[name_table_count], name, length);
    name_table_names[name_table_count][length] = '\0';
    return (uint8_t)name_table_count++;
}

static void identifier(ms_parser_t* parser) {
    ms_token_t name = parser->previous;
    uint8_t name_index = add_name(name.start, name.length);
    
    // 检查是否是赋值
    if (match(parser, TOKEN_EQUAL)) {
        // 赋值
        expression(parser);
        
        int arg = resolve_local(parser, &name);
        if (arg != -1) {
            emit_bytes(parser, OP_SET_LOCAL, (uint8_t)arg);
        } else {
            // Python风格：首次赋值即定义
            emit_bytes(parser, OP_DEFINE_GLOBAL, name_index);
        }
    } else {
        // 读取
        int arg = resolve_local(parser, &name);
        if (arg != -1) {
            emit_bytes(parser, OP_GET_LOCAL, (uint8_t)arg);
        } else {
            emit_bytes(parser, OP_GET_GLOBAL, name_index);
        }
    }
}

static void number(ms_parser_t* parser) {
    double value = strtod(parser->previous.start, NULL);
    // 检查是否为整数
    if (value == (int64_t)value) {
        emit_constant(parser, ms_value_int((int64_t)value));
    } else {
        emit_constant(parser, ms_value_float(value));
    }
}

static void string(ms_parser_t* parser) {
    // 检查是否是三引号字符串
    int quote_len = 1;  // 默认单引号
    if (parser->previous.length >= 6 &&  // 至少 """..."""
        parser->previous.start[0] == '"' &&
        parser->previous.start[1] == '"' &&
        parser->previous.start[2] == '"') {
        quote_len = 3;  // 三引号
    }
    
    // 去掉引号
    int str_len = parser->previous.length - 2 * quote_len;
    char* str = malloc(str_len + 1);
    memcpy(str, parser->previous.start + quote_len, str_len);
    str[str_len] = '\0';
    emit_constant(parser, ms_value_string(str));
    free(str);
}

// Parse f-string: f"text {expr} more text"
static void fstring(ms_parser_t* parser) {
    const char* start = parser->previous.start;
    int total_length = parser->previous.length;
    
    // Skip 'f' or 'F' and opening quote
    const char* content_start = start + 2;
    int content_length = total_length - 3;  // Remove f, opening quote, closing quote
    
    // Create a buffer for the f-string content
    char* content = malloc(content_length + 1);
    memcpy(content, content_start, content_length);
    content[content_length] = '\0';
    
    // Parse the f-string and collect parts
    int parts_count = 0;
    int pos = 0;
    
    for (int i = 0; i < content_length; i++) {
        if (content[i] == '{') {
            // Found expression start
            if (i > pos) {
                // Emit string part before expression
                char* str_part = malloc(i - pos + 1);
                memcpy(str_part, content + pos, i - pos);
                str_part[i - pos] = '\0';
                emit_constant(parser, ms_value_string(str_part));
                free(str_part);
                parts_count++;
            }
            
            // Find matching closing brace
            int brace_depth = 1;
            int expr_start = i + 1;
            int expr_end = i + 1;
            
            while (expr_end < content_length && brace_depth > 0) {
                if (content[expr_end] == '{') brace_depth++;
                else if (content[expr_end] == '}') brace_depth--;
                if (brace_depth > 0) expr_end++;
            }
            
            if (brace_depth != 0) {
                free(content);
                error(parser, "Unterminated expression in f-string.");
                return;
            }
            
            // Extract and parse the expression
            int expr_length = expr_end - expr_start;
            char* expr_str = malloc(expr_length + 1);
            memcpy(expr_str, content + expr_start, expr_length);
            expr_str[expr_length] = '\0';
            
            // Create a new lexer for the expression
            ms_lexer_t expr_lexer;
            ms_lexer_init(&expr_lexer, expr_str);
            
            // Create a new parser for the expression
            ms_parser_t expr_parser;
            expr_parser.lexer = &expr_lexer;
            expr_parser.compiling_chunk = parser->compiling_chunk;
            expr_parser.current.type = TOKEN_EOF;
            expr_parser.current.start = "";
            expr_parser.current.length = 0;
            expr_parser.current.line = 0;
            expr_parser.current.column = 0;
            expr_parser.previous.type = TOKEN_EOF;
            expr_parser.previous.start = "";
            expr_parser.previous.length = 0;
            expr_parser.previous.line = 0;
            expr_parser.previous.column = 0;
            expr_parser.had_error = false;
            expr_parser.panic_mode = false;
            expr_parser.function_chunk_count = 0;
            
            // Parse the expression
            advance(&expr_parser);
            expression(&expr_parser);
            
            // Convert the result to string using str() function
            // We'll emit a call to the built-in str() function
            // For now, we'll use a simple approach: wrap with str() call
            // Actually, let's just emit the value and let OP_ADD handle it
            // But we need to ensure it's a string, so we'll use a conversion
            
            free(expr_str);
            parts_count++;
            
            pos = expr_end + 1;
            i = expr_end;
        }
    }
    
    // Emit remaining string part
    if (pos < content_length) {
        char* str_part = malloc(content_length - pos + 1);
        memcpy(str_part, content + pos, content_length - pos);
        str_part[content_length - pos] = '\0';
        emit_constant(parser, ms_value_string(str_part));
        free(str_part);
        parts_count++;
    }
    
    // If no parts, emit empty string
    if (parts_count == 0) {
        emit_constant(parser, ms_value_string(""));
    } else if (parts_count > 1) {
        // Emit ADD operations to concatenate all parts
        for (int i = 1; i < parts_count; i++) {
            emit_byte(parser, OP_ADD);
        }
    }
    
    free(content);
}

static void unary(ms_parser_t* parser) {
    ms_token_type_t operator_type = parser->previous.type;
    
    // 编译操作数
    parse_precedence(parser, PREC_UNARY);
    
    // 发出操作符指令
    switch (operator_type) {
        case TOKEN_BANG: emit_byte(parser, OP_NOT); break;
        case TOKEN_NOT: emit_byte(parser, OP_NOT); break;
        case TOKEN_MINUS: emit_byte(parser, OP_NEGATE); break;
        default: return; // Unreachable.
    }
}

static void and_(ms_parser_t* parser) {
    int end_jump = emit_jump(parser, OP_JUMP_IF_FALSE);
    
    emit_byte(parser, OP_POP);
    parse_precedence(parser, PREC_AND);
    
    patch_jump(parser, end_jump);
}

static void or_(ms_parser_t* parser) {
    int else_jump = emit_jump(parser, OP_JUMP_IF_FALSE);
    int end_jump = emit_jump(parser, OP_JUMP);
    
    patch_jump(parser, else_jump);
    emit_byte(parser, OP_POP);
    
    parse_precedence(parser, PREC_OR);
    patch_jump(parser, end_jump);
}

ms_parse_rule_t rules[] = {
    [TOKEN_LEFT_PAREN]    = {grouping, call,      PREC_CALL},
    [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,      PREC_NONE},
    [TOKEN_LEFT_BRACE]    = {dict_literal, NULL,  PREC_NONE}, 
    [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,      PREC_NONE},
    [TOKEN_LEFT_BRACKET]  = {list_literal, index_access, PREC_CALL},
    [TOKEN_RIGHT_BRACKET] = {NULL,     NULL,      PREC_NONE},
    [TOKEN_COMMA]         = {NULL,     NULL,      PREC_NONE},
    [TOKEN_DOT]           = {NULL,     attribute, PREC_CALL},
    [TOKEN_MINUS]         = {unary,    binary,    PREC_TERM},
    [TOKEN_PLUS]          = {NULL,     binary,    PREC_TERM},
    [TOKEN_SEMICOLON]     = {NULL,     NULL,      PREC_NONE},
    [TOKEN_SLASH]         = {NULL,     binary,    PREC_FACTOR},
    [TOKEN_STAR]          = {NULL,     binary,    PREC_FACTOR},
    [TOKEN_SLASH_SLASH]   = {NULL,     binary,    PREC_FACTOR},
    [TOKEN_STAR_STAR]     = {NULL,     binary,    PREC_POWER},
    [TOKEN_PERCENT]       = {NULL,     binary,    PREC_FACTOR},
    [TOKEN_BANG]          = {unary,    NULL,      PREC_NONE},
    [TOKEN_BANG_EQUAL]    = {NULL,     binary,    PREC_EQUALITY},
    [TOKEN_EQUAL]         = {NULL,     NULL,      PREC_NONE},
    [TOKEN_EQUAL_EQUAL]   = {NULL,     binary,    PREC_EQUALITY},
    [TOKEN_GREATER]       = {NULL,     binary,    PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL,     binary,    PREC_COMPARISON},
    [TOKEN_LESS]          = {NULL,     binary,    PREC_COMPARISON},
    [TOKEN_LESS_EQUAL]    = {NULL,     binary,    PREC_COMPARISON},
    [TOKEN_IDENTIFIER]    = {identifier, NULL,    PREC_NONE},
    [TOKEN_STRING]        = {string,   NULL,      PREC_NONE},
    [TOKEN_FSTRING]       = {fstring,  NULL,      PREC_NONE},
    [TOKEN_NUMBER]        = {number,   NULL,      PREC_NONE},
    [TOKEN_AND]           = {NULL,     and_,      PREC_AND},
    [TOKEN_CLASS]         = {NULL,     NULL,      PREC_NONE},
    [TOKEN_ELSE]          = {NULL,     NULL,      PREC_NONE},
    [TOKEN_FALSE]         = {literal,  NULL,      PREC_NONE},
    [TOKEN_FOR]           = {NULL,     NULL,      PREC_NONE},
    [TOKEN_FUNC]          = {NULL,     NULL,      PREC_NONE},
    [TOKEN_IF]            = {NULL,     NULL,      PREC_NONE},
    [TOKEN_NIL]           = {literal,  NULL,      PREC_NONE},
    [TOKEN_NONE]          = {literal,  NULL,      PREC_NONE},
    [TOKEN_OR]            = {NULL,     or_,       PREC_OR},
    [TOKEN_RETURN]        = {NULL,     NULL,      PREC_NONE},
    [TOKEN_TRUE]          = {literal,  NULL,      PREC_NONE},
    [TOKEN_VAR]           = {NULL,     NULL,      PREC_NONE},
    [TOKEN_WHILE]         = {NULL,     NULL,      PREC_NONE},
    [TOKEN_DEF]           = {NULL,     NULL,      PREC_NONE},
    [TOKEN_PASS]          = {NULL,     NULL,      PREC_NONE},
    [TOKEN_IN]            = {NULL,     NULL,      PREC_NONE},
    [TOKEN_NOT]           = {unary,    NULL,      PREC_NONE},
    [TOKEN_IS]            = {NULL,     NULL,      PREC_NONE},
    [TOKEN_ERROR]         = {NULL,     NULL,      PREC_NONE},
    [TOKEN_EOF]           = {NULL,     NULL,      PREC_NONE},
};

static void parse_precedence(ms_parser_t* parser, ms_precedence_t precedence) {
    advance(parser);
    ms_parse_fn_t prefix_rule = get_rule(parser->previous.type)->prefix;
    if (prefix_rule == NULL) {
        error(parser, "Expect expression.");
        return;
    }
    
    prefix_rule(parser);
    
    while (precedence <= get_rule(parser->current.type)->precedence) {
        advance(parser);
        ms_parse_fn_t infix_rule = get_rule(parser->previous.type)->infix;
        infix_rule(parser);
    }
    
    // 检查三元表达式: value_if_true if condition else value_if_false
    if (precedence <= PREC_TERNARY && check(parser, TOKEN_IF)) {
        advance(parser);  // 消费 if
        expression(parser);  // 解析条件
        consume(parser, TOKEN_ELSE, "Expect 'else' in ternary expression.");
        parse_precedence(parser, PREC_TERNARY);  // 解析 value_if_false
        emit_byte(parser, OP_TERNARY);
    }
}

static ms_parse_rule_t* get_rule(ms_token_type_t type) {
    return &rules[type];
}

static void expression(ms_parser_t* parser) {
    parse_precedence(parser, PREC_ASSIGNMENT);
}

static void expression_statement(ms_parser_t* parser) {
    expression(parser);
    
    // 消费语句后的换行符（如果有）
    if (match(parser, TOKEN_NEWLINE)) {
        // 换行符已消费
    } else if (!check(parser, TOKEN_EOF) && !check(parser, TOKEN_DEDENT)) {
        error(parser, "Expect newline after expression.");
    }
    
    emit_byte(parser, OP_POP);
}

static void var_declaration(ms_parser_t* parser) {
    uint8_t global = parse_variable(parser, "Expect variable name.");
    
    if (match(parser, TOKEN_EQUAL)) {
        expression(parser);
    } else {
        emit_byte(parser, OP_NIL);
    }
    
    // 消费语句后的换行符（如果有）
    if (match(parser, TOKEN_NEWLINE)) {
        // 换行符已消费
    } else if (!check(parser, TOKEN_EOF) && !check(parser, TOKEN_DEDENT)) {
        error(parser, "Expect newline after variable declaration.");
    }
    
    define_variable(parser, global);
}

static void if_statement(ms_parser_t* parser) {
    // if condition:
    expression(parser);
    consume(parser, TOKEN_COLON, "Expect ':' after if condition.");
    consume(parser, TOKEN_NEWLINE, "Expect newline after ':'.");
    
    int then_jump = emit_jump(parser, OP_JUMP_IF_FALSE);
    emit_byte(parser, OP_POP);
    
    begin_scope();
    skip_newlines(parser);  // 跳过空行
    consume(parser, TOKEN_INDENT, "Expect indentation after ':'.");
    
    while (!check(parser, TOKEN_DEDENT) && !check(parser, TOKEN_EOF)) {
        skip_newlines(parser);
        if (check(parser, TOKEN_DEDENT) || check(parser, TOKEN_EOF)) break;
        declaration(parser);
    }
    
    if (!check(parser, TOKEN_EOF)) {
        consume(parser, TOKEN_DEDENT, "Expect dedent after block.");
    }
    end_scope(parser);
    
    // 执行if块后，跳到最后
    int exit_jumps[256];
    int exit_jump_count = 0;
    exit_jumps[exit_jump_count++] = emit_jump(parser, OP_JUMP);
    
    // patch if条件为假时的跳转
    patch_jump(parser, then_jump);
    emit_byte(parser, OP_POP);
    
    // 处理 elif
    while (match(parser, TOKEN_ELIF)) {
        expression(parser);
        consume(parser, TOKEN_COLON, "Expect ':' after elif condition.");
        consume(parser, TOKEN_NEWLINE, "Expect newline after ':'.");
        
        int elif_jump = emit_jump(parser, OP_JUMP_IF_FALSE);
        emit_byte(parser, OP_POP);
        
        begin_scope();
        skip_newlines(parser);  // 跳过空行
        consume(parser, TOKEN_INDENT, "Expect indentation after ':'.");
        
        while (!check(parser, TOKEN_DEDENT) && !check(parser, TOKEN_EOF)) {
            skip_newlines(parser);
            if (check(parser, TOKEN_DEDENT) || check(parser, TOKEN_EOF)) break;
            declaration(parser);
        }
        
        if (!check(parser, TOKEN_EOF)) {
            consume(parser, TOKEN_DEDENT, "Expect dedent after block.");
        }
        end_scope(parser);
        
        // elif块执行后，跳到最后
        exit_jumps[exit_jump_count++] = emit_jump(parser, OP_JUMP);
        
        // patch elif条件为假时的跳转到下一个elif或else
        patch_jump(parser, elif_jump);
        emit_byte(parser, OP_POP);
    }
    
    if (match(parser, TOKEN_ELSE)) {
        consume(parser, TOKEN_COLON, "Expect ':' after else.");
        consume(parser, TOKEN_NEWLINE, "Expect newline after ':'.");
        
        begin_scope();
        skip_newlines(parser);  // 跳过空行
        consume(parser, TOKEN_INDENT, "Expect indentation after ':'.");
        
        while (!check(parser, TOKEN_DEDENT) && !check(parser, TOKEN_EOF)) {
            skip_newlines(parser);
            if (check(parser, TOKEN_DEDENT) || check(parser, TOKEN_EOF)) break;
            declaration(parser);
        }
        
        if (!check(parser, TOKEN_EOF)) {
            consume(parser, TOKEN_DEDENT, "Expect dedent after block.");
        }
        end_scope(parser);
    }
    
    // patch所有的exit jumps到这里
    for (int i = 0; i < exit_jump_count; i++) {
        patch_jump(parser, exit_jumps[i]);
    }
}

static void while_statement(ms_parser_t* parser) {
    int loop_start = current_chunk(parser)->count;
    
    // 进入循环，增加循环深度
    loop_depth++;
    int saved_break_count = break_count;
    int saved_continue_count = continue_count;
    // 不要重置计数器，而是从当前位置继续
    // break_count = 0;
    // continue_count = 0;
    
    expression(parser);
    consume(parser, TOKEN_COLON, "Expect ':' after while condition.");
    consume(parser, TOKEN_NEWLINE, "Expect newline after ':'.");
    
    int exit_jump = emit_jump(parser, OP_JUMP_IF_FALSE);
    emit_byte(parser, OP_POP);
    
    begin_scope();
    skip_newlines(parser);  // 跳过空行
    consume(parser, TOKEN_INDENT, "Expect indentation after ':'.");
    
    while (!check(parser, TOKEN_DEDENT) && !check(parser, TOKEN_EOF)) {
        skip_newlines(parser);
        if (check(parser, TOKEN_DEDENT) || check(parser, TOKEN_EOF)) break;
        declaration(parser);
    }
    
    if (!check(parser, TOKEN_EOF)) {
        consume(parser, TOKEN_DEDENT, "Expect dedent after block.");
    }
    end_scope(parser);
    
    // patch 当前循环的 continue 跳转到循环开始
    for (int i = saved_continue_count; i < continue_count; i++) {
        patch_jump(parser, continue_jumps[i]);
    }
    
    emit_loop(parser, loop_start);
    
    patch_jump(parser, exit_jump);
    emit_byte(parser, OP_POP);
    
    // patch 当前循环的 break 跳转到循环结束
    for (int i = saved_break_count; i < break_count; i++) {
        patch_jump(parser, break_jumps[i]);
    }
    
    // 恢复循环深度和计数器
    loop_depth--;
    break_count = saved_break_count;
    continue_count = saved_continue_count;
}

static void for_statement(ms_parser_t* parser) {
    begin_scope();
    
    // 进入循环，增加循环深度
    loop_depth++;
    int saved_break_count = break_count;
    int saved_continue_count = continue_count;
    // 不要重置计数器，而是从当前位置继续
    // break_count = 0;
    // continue_count = 0;
    
    // for var in iterable:
    consume(parser, TOKEN_IDENTIFIER, "Expect variable name.");
    ms_token_t var_name = parser->previous;
    
    consume(parser, TOKEN_IN, "Expect 'in' after variable.");
    
    // Add loop variable as local (slot 0) - initialize with nil
    emit_byte(parser, OP_NIL);
    add_local(parser, var_name);
    mark_initialized();
    uint8_t var_slot = local_count - 1;
    
    // Parse the iterable expression (leaves value on stack)
    expression(parser);
    
    // Store iterable in a local variable (slot 1) - value is already on stack
    add_local(parser, (ms_token_t){.start = "__iter__", .length = 8});
    mark_initialized();
    uint8_t iter_slot = local_count - 1;
    
    // Initialize index to 0 and store in a local variable (slot 2) - value is already on stack
    emit_constant(parser, ms_value_int(0));
    add_local(parser, (ms_token_t){.start = "__index__", .length = 9});
    mark_initialized();
    uint8_t index_slot = local_count - 1;
    
    consume(parser, TOKEN_COLON, "Expect ':' after for clause.");
    consume(parser, TOKEN_NEWLINE, "Expect newline after ':'.");
    skip_newlines(parser);  // 跳过空行
    consume(parser, TOKEN_INDENT, "Expect indent after for:");
    
    // Loop start
    int loop_start = current_chunk(parser)->count;
    
    // Emit FOR_ITER_LOCAL instruction
    // This will check if there are more elements and set the loop variable
    emit_bytes(parser, OP_FOR_ITER_LOCAL, var_slot);
    emit_byte(parser, iter_slot);
    emit_byte(parser, index_slot);
    
    // Jump if no more elements
    int exit_jump = emit_jump(parser, OP_JUMP_IF_FALSE);
    emit_byte(parser, OP_POP);  // Pop the boolean result
    
    // Parse loop body
    while (!check(parser, TOKEN_DEDENT) && !check(parser, TOKEN_EOF)) {
        skip_newlines(parser);
        if (check(parser, TOKEN_DEDENT) || check(parser, TOKEN_EOF)) break;
        declaration(parser);
    }
    
    if (!check(parser, TOKEN_EOF)) {
        consume(parser, TOKEN_DEDENT, "Expect dedent after for block.");
    }
    
    // patch 当前循环的 continue 跳转到循环开始
    for (int i = saved_continue_count; i < continue_count; i++) {
        patch_jump(parser, continue_jumps[i]);
    }
    
    // Loop back
    emit_loop(parser, loop_start);
    
    // Patch exit jump
    patch_jump(parser, exit_jump);
    emit_byte(parser, OP_POP);  // Pop the boolean result
    
    // patch 当前循环的 break 跳转到循环结束
    for (int i = saved_break_count; i < break_count; i++) {
        patch_jump(parser, break_jumps[i]);
    }
    
    // 恢复循环深度和计数器
    loop_depth--;
    break_count = saved_break_count;
    continue_count = saved_continue_count;
    
    end_scope(parser);
}

static void with_statement(ms_parser_t* parser) {
    // with expression as variable:
    expression(parser);
    
    consume(parser, TOKEN_AS, "Expect 'as' after with expression.");
    consume(parser, TOKEN_IDENTIFIER, "Expect variable name after 'as'.");
    
    uint8_t var_index = add_name(parser->previous.start, parser->previous.length);
    
    consume(parser, TOKEN_COLON, "Expect ':' after with clause.");
    consume(parser, TOKEN_NEWLINE, "Expect newline after ':'.");
    
    // 将表达式结果存储到变量
    emit_bytes(parser, OP_DEFINE_GLOBAL, var_index);
    
    begin_scope();
    skip_newlines(parser);  // 跳过空行
    consume(parser, TOKEN_INDENT, "Expect indentation after ':'.");
    
    while (!check(parser, TOKEN_DEDENT) && !check(parser, TOKEN_EOF)) {
        skip_newlines(parser);
        if (check(parser, TOKEN_DEDENT) || check(parser, TOKEN_EOF)) break;
        declaration(parser);
    }
    
    if (!check(parser, TOKEN_EOF)) {
        consume(parser, TOKEN_DEDENT, "Expect dedent after block.");
    }
    end_scope(parser);
}

static void import_statement(ms_parser_t* parser) {
    // import module
    // import module as alias
    // from module import name
    // from module import name as alias
    
    if (match(parser, TOKEN_FROM)) {
        // from module import name
        consume(parser, TOKEN_IDENTIFIER, "Expect module name after 'from'.");
        ms_token_t module_name = parser->previous;
        
        consume(parser, TOKEN_IMPORT, "Expect 'import' after module name.");
        consume(parser, TOKEN_IDENTIFIER, "Expect name to import.");
        ms_token_t import_name = parser->previous;
        
        uint8_t var_index = add_name(import_name.start, import_name.length);
        
        if (match(parser, TOKEN_AS)) {
            consume(parser, TOKEN_IDENTIFIER, "Expect alias name after 'as'.");
            var_index = add_name(parser->previous.start, parser->previous.length);
        }
        
        // 加载模块
        uint8_t module_index = add_name(module_name.start, module_name.length);
        emit_bytes(parser, OP_LOAD_MODULE, module_index);
        emit_bytes(parser, OP_DEFINE_GLOBAL, var_index);
    } else {
        // import module
        consume(parser, TOKEN_IDENTIFIER, "Expect module name after 'import'.");
        ms_token_t module_name = parser->previous;
        
        uint8_t var_index = add_name(module_name.start, module_name.length);
        
        if (match(parser, TOKEN_AS)) {
            consume(parser, TOKEN_IDENTIFIER, "Expect alias name after 'as'.");
            var_index = add_name(parser->previous.start, parser->previous.length);
        }
        
        // 加载模块
        uint8_t module_index = add_name(module_name.start, module_name.length);
        emit_bytes(parser, OP_LOAD_MODULE, module_index);
        emit_bytes(parser, OP_DEFINE_GLOBAL, var_index);
    }
    
    // 消费语句后的换行符（如果有）
    if (match(parser, TOKEN_NEWLINE)) {
        // 换行符已消费
    } else if (!check(parser, TOKEN_EOF) && !check(parser, TOKEN_DEDENT)) {
        error(parser, "Expect newline after import.");
    }
}

static int emit_jump(ms_parser_t* parser, uint8_t instruction) {
    emit_byte(parser, instruction);
    emit_byte(parser, 0xff);
    emit_byte(parser, 0xff);
    return current_chunk(parser)->count - 2;
}

static void patch_jump(ms_parser_t* parser, int offset) {
    int jump = current_chunk(parser)->count - offset - 2;
    
    if (jump > UINT16_MAX) {
        error(parser, "Too much code to jump over.");
    }
    
    current_chunk(parser)->code[offset] = (jump >> 8) & 0xff;
    current_chunk(parser)->code[offset + 1] = jump & 0xff;
}

static void emit_loop(ms_parser_t* parser, int loop_start) {
    emit_byte(parser, OP_LOOP);
    
    int offset = current_chunk(parser)->count - loop_start + 2;
    if (offset > UINT16_MAX) error(parser, "Loop body too large.");
    
    emit_byte(parser, (offset >> 8) & 0xff);
    emit_byte(parser, offset & 0xff);
}

static void match_statement(ms_parser_t* parser) {
    // match expression:
    //     case pattern1:
    //         statements
    //     case pattern2:
    //         statements
    //     case _:
    //         statements
    
    expression(parser);
    consume(parser, TOKEN_COLON, "Expect ':' after match expression.");
    consume(parser, TOKEN_NEWLINE, "Expect newline after ':'.");
    skip_newlines(parser);
    consume(parser, TOKEN_INDENT, "Expect indent after match:");
    
    int exit_jumps[256];
    int exit_jump_count = 0;
    
    while (match(parser, TOKEN_CASE)) {
        // case pattern:
        // 对于简单实现，我们支持字面量和通配符 _
        
        if (check(parser, TOKEN_IDENTIFIER) && parser->current.length == 1 && parser->current.start[0] == '_') {
            // 通配符 _，总是匹配
            advance(parser);
            emit_byte(parser, OP_POP);  // 弹出之前的比较结果
            emit_byte(parser, OP_TRUE);  // 推送 true
        } else {
            // 字面量或表达式
            // 复制栈顶的值用于比较
            emit_byte(parser, OP_DUP);
            expression(parser);
            emit_byte(parser, OP_EQUAL);
        }
        
        consume(parser, TOKEN_COLON, "Expect ':' after case pattern.");
        consume(parser, TOKEN_NEWLINE, "Expect newline after ':'.");
        
        int case_jump = emit_jump(parser, OP_JUMP_IF_FALSE);
        emit_byte(parser, OP_POP);  // 弹出比较结果
        emit_byte(parser, OP_POP);  // 弹出原始值
        
        begin_scope();
        skip_newlines(parser);
        consume(parser, TOKEN_INDENT, "Expect indent after case:");
        
        while (!check(parser, TOKEN_DEDENT) && !check(parser, TOKEN_EOF)) {
            skip_newlines(parser);
            if (check(parser, TOKEN_DEDENT) || check(parser, TOKEN_EOF)) break;
            declaration(parser);
        }
        
        if (!check(parser, TOKEN_EOF)) {
            consume(parser, TOKEN_DEDENT, "Expect dedent after case block.");
        }
        end_scope(parser);
        
        // case 块执行后，跳到 match 结束
        exit_jumps[exit_jump_count++] = emit_jump(parser, OP_JUMP);
        
        // patch case 条件为假时的跳转到下一个 case
        patch_jump(parser, case_jump);
        emit_byte(parser, OP_POP);  // 弹出比较结果
        
        skip_newlines(parser);
    }
    
    // 如果没有任何 case 匹配，弹出原始值
    emit_byte(parser, OP_POP);
    
    if (!check(parser, TOKEN_EOF)) {
        consume(parser, TOKEN_DEDENT, "Expect dedent after match block.");
    }
    
    // patch 所有的 exit jumps 到这里
    for (int i = 0; i < exit_jump_count; i++) {
        patch_jump(parser, exit_jumps[i]);
    }
}

static void statement(ms_parser_t* parser) {
    if (match(parser, TOKEN_IF)) {
        if_statement(parser);
    } else if (match(parser, TOKEN_MATCH)) {
        match_statement(parser);
    } else if (match(parser, TOKEN_WHILE)) {
        while_statement(parser);
    } else if (match(parser, TOKEN_FOR)) {
        for_statement(parser);
    } else if (match(parser, TOKEN_WITH)) {
        with_statement(parser);
    } else if (match(parser, TOKEN_BREAK)) {
        // break 语句
        if (loop_depth == 0) {
            error(parser, "'break' outside loop.");
            return;
        }
        
        // 记录 break 跳转位置
        if (break_count >= 256) {
            error(parser, "Too many break statements.");
            return;
        }
        break_jumps[break_count++] = emit_jump(parser, OP_JUMP);
        
        // 消费换行符
        if (match(parser, TOKEN_NEWLINE)) {
            // 换行符已消费
        } else if (!check(parser, TOKEN_EOF) && !check(parser, TOKEN_DEDENT)) {
            error(parser, "Expect newline after 'break'.");
        }
    } else if (match(parser, TOKEN_CONTINUE)) {
        // continue 语句
        if (loop_depth == 0) {
            error(parser, "'continue' outside loop.");
            return;
        }
        
        // 记录 continue 跳转位置
        if (continue_count >= 256) {
            error(parser, "Too many continue statements.");
            return;
        }
        continue_jumps[continue_count++] = emit_jump(parser, OP_JUMP);
        
        // 消费换行符
        if (match(parser, TOKEN_NEWLINE)) {
            // 换行符已消费
        } else if (!check(parser, TOKEN_EOF) && !check(parser, TOKEN_DEDENT)) {
            error(parser, "Expect newline after 'continue'.");
        }
    } else if (match(parser, TOKEN_PASS)) {
        // 消费语句后的换行符（如果有）
        if (match(parser, TOKEN_NEWLINE)) {
            // 换行符已消费
        } else if (!check(parser, TOKEN_EOF) && !check(parser, TOKEN_DEDENT)) {
            error(parser, "Expect newline after 'pass'.");
        }
    } else if (match(parser, TOKEN_RETURN)) {
        // return 语句
        if (check(parser, TOKEN_NEWLINE) || check(parser, TOKEN_EOF) || check(parser, TOKEN_DEDENT)) {
            // return 无值
            emit_byte(parser, OP_NIL);
        } else {
            // return 有值
            expression(parser);
        }
        emit_byte(parser, OP_RETURN);
        
        // 消费语句后的换行符（如果有）
        if (match(parser, TOKEN_NEWLINE)) {
            // 换行符已消费
        } else if (!check(parser, TOKEN_EOF) && !check(parser, TOKEN_DEDENT)) {
            error(parser, "Expect newline after return.");
        }
    } else {
        expression_statement(parser);
    }
}

static void function_declaration(ms_parser_t* parser) {
    // def name(params):
    uint8_t name_index = parse_variable(parser, "Expect function name.");
    
    consume(parser, TOKEN_LEFT_PAREN, "Expect '(' after function name.");
    
    // 解析参数
    int param_count = 0;
    ms_token_t params[255];
    if (!check(parser, TOKEN_RIGHT_PAREN)) {
        do {
            if (param_count >= 255) {
                error(parser, "Can't have more than 255 parameters.");
            }
            
            consume(parser, TOKEN_IDENTIFIER, "Expect parameter name.");
            params[param_count] = parser->previous;
            param_count++;
        } while (match(parser, TOKEN_COMMA));
    }
    
    consume(parser, TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
    consume(parser, TOKEN_COLON, "Expect ':' after function signature.");
    consume(parser, TOKEN_NEWLINE, "Expect newline after ':'.");
    
    // 跳过函数体开始前的空行
    skip_newlines(parser);
    
    consume(parser, TOKEN_INDENT, "Expect indentation after ':'.");
    ms_chunk_t* function_chunk = create_function_chunk(parser);
    if (function_chunk == NULL) {
        error(parser, "Too many functions.");
        return;
    }
    
    // 保存当前chunk并切换到函数chunk
    ms_chunk_t* prev_chunk = parser->compiling_chunk;
    parser->compiling_chunk = function_chunk;
    
    // 保存当前的局部变量状态
    ms_local_t saved_locals[256];
    int saved_local_count = local_count;
    int saved_scope_depth = scope_depth;
    memcpy(saved_locals, locals, sizeof(locals));
    local_count = 0;
    scope_depth = 0;
    
    // 编译函数体
    begin_scope();
    
    // 添加参数作为局部变量
    for (int i = 0; i < param_count; i++) {
        add_local(parser, params[i]);
        mark_initialized();
    }
    
    while (!check(parser, TOKEN_DEDENT) && !check(parser, TOKEN_EOF)) {
        skip_newlines(parser);
        if (check(parser, TOKEN_DEDENT) || check(parser, TOKEN_EOF)) break;
        declaration(parser);
    }
    
    if (!check(parser, TOKEN_EOF)) {
        consume(parser, TOKEN_DEDENT, "Expect dedent after block.");
    }
    
    // 如果函数体没有return，添加return nil
    emit_byte(parser, OP_NIL);
    emit_byte(parser, OP_RETURN);
    
    end_scope(parser);
    
    // 恢复之前的chunk和局部变量状态
    parser->compiling_chunk = prev_chunk;
    local_count = saved_local_count;
    scope_depth = saved_scope_depth;
    memcpy(locals, saved_locals, sizeof(locals));
    
    // 创建函数对象并存储为常量
    ms_function_t* function = malloc(sizeof(ms_function_t));
    function->chunk = function_chunk;
    function->arity = param_count;
    function->name = malloc(name_table_names[name_index] ? strlen(name_table_names[name_index]) + 1 : 1);
    if (name_table_names[name_index]) {
        strcpy(function->name, name_table_names[name_index]);
    } else {
        function->name[0] = '\0';
    }
    
    ms_value_t func_value;
    func_value.type = MS_VAL_FUNCTION;
    func_value.as.function = function;
    
    // 发出OP_FUNCTION指令
    uint8_t func_const = make_constant(parser, func_value);
    emit_bytes(parser, OP_CONSTANT, func_const);
    
    // 定义函数变量
    define_variable(parser, name_index);
}

static void declaration(ms_parser_t* parser) {
    if (match(parser, TOKEN_VAR)) {
        var_declaration(parser);
    } else if (match(parser, TOKEN_DEF)) {
        function_declaration(parser);
    } else if (match(parser, TOKEN_IMPORT) || check(parser, TOKEN_FROM)) {
        import_statement(parser);
    } else {
        statement(parser);
    }
    
    if (parser->panic_mode) {
        // 同步到下一个语句
        while (parser->current.type != TOKEN_EOF) {
            if (parser->previous.type == TOKEN_NEWLINE) return;
            
            switch (parser->current.type) {
                case TOKEN_CLASS:
                case TOKEN_DEF:
                case TOKEN_VAR:
                case TOKEN_FOR:
                case TOKEN_IF:
                case TOKEN_WHILE:
                case TOKEN_RETURN:
                case TOKEN_IMPORT:
                    return;
                default:
                    ; // Do nothing.
            }
            
            advance(parser);
        }
    }
}

void ms_parser_init(ms_parser_t* parser, ms_lexer_t* lexer) {
    parser->had_error = false;
    parser->panic_mode = false;
    parser->lexer = lexer;
    parser->function_chunk_count = 0;
}

bool ms_compile(const char* source, ms_chunk_t* chunk) {
    ms_lexer_t lexer;
    ms_lexer_init(&lexer, source);
    
    ms_parser_t parser;
    ms_parser_init(&parser, &lexer);
    parser.compiling_chunk = chunk;
    
    advance(&parser);
    
    // 跳过开头的换行符
    while (match(&parser, TOKEN_NEWLINE)) {
        // Skip
    }
    
    while (!match(&parser, TOKEN_EOF)) {
        declaration(&parser);
        // 跳过多余的换行符
        while (match(&parser, TOKEN_NEWLINE)) {
            // Skip
        }
    }
    
    end_compiler(&parser);
    return !parser.had_error;
}