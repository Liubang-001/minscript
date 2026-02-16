#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ms_chunk_t* current_chunk(ms_parser_t* parser) {
    return parser->compiling_chunk;
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

static void end_compiler(ms_parser_t* parser) {
    emit_return(parser);
}

// 前向声明
static void expression(ms_parser_t* parser);
static void statement(ms_parser_t* parser);
static void declaration(ms_parser_t* parser);
static ms_parse_rule_t* get_rule(ms_token_type_t type);
static void parse_precedence(ms_parser_t* parser, ms_precedence_t precedence);

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
        default: return; // Unreachable.
    }
}

static void literal(ms_parser_t* parser) {
    switch (parser->previous.type) {
        case TOKEN_FALSE: emit_byte(parser, OP_FALSE); break;
        case TOKEN_NIL: emit_byte(parser, OP_NIL); break;
        case TOKEN_TRUE: emit_byte(parser, OP_TRUE); break;
        default: return; // Unreachable.
    }
}

static void grouping(ms_parser_t* parser) {
    expression(parser);
    consume(parser, TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void number(ms_parser_t* parser) {
    double value = strtod(parser->previous.start, NULL);
    emit_constant(parser, ms_value_float(value));
}

static void string(ms_parser_t* parser) {
    // 去掉引号
    char* str = malloc(parser->previous.length - 1);
    memcpy(str, parser->previous.start + 1, parser->previous.length - 2);
    str[parser->previous.length - 2] = '\0';
    emit_constant(parser, ms_value_string(str));
    free(str);
}

static void unary(ms_parser_t* parser) {
    ms_token_type_t operator_type = parser->previous.type;
    
    // 编译操作数
    parse_precedence(parser, PREC_UNARY);
    
    // 发出操作符指令
    switch (operator_type) {
        case TOKEN_BANG: emit_byte(parser, OP_NOT); break;
        case TOKEN_MINUS: emit_byte(parser, OP_NEGATE); break;
        default: return; // Unreachable.
    }
}

ms_parse_rule_t rules[] = {
    [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
    [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE}, 
    [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
    [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
    [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
    [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
    [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
    [TOKEN_BANG]          = {unary,    NULL,   PREC_NONE},
    [TOKEN_BANG_EQUAL]    = {NULL,     binary, PREC_EQUALITY},
    [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EQUAL_EQUAL]   = {NULL,     binary, PREC_EQUALITY},
    [TOKEN_GREATER]       = {NULL,     binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL,     binary, PREC_COMPARISON},
    [TOKEN_LESS]          = {NULL,     binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL]    = {NULL,     binary, PREC_COMPARISON},
    [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_STRING]        = {string,   NULL,   PREC_NONE},
    [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
    [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FALSE]         = {literal,  NULL,   PREC_NONE},
    [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FUNC]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_NIL]           = {literal,  NULL,   PREC_NONE},
    [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
    [TOKEN_TRUE]          = {literal,  NULL,   PREC_NONE},
    [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
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
}

static ms_parse_rule_t* get_rule(ms_token_type_t type) {
    return &rules[type];
}

static void expression(ms_parser_t* parser) {
    parse_precedence(parser, PREC_ASSIGNMENT);
}

static void print_statement(ms_parser_t* parser) {
    expression(parser);
    consume(parser, TOKEN_NEWLINE, "Expect newline after value.");
    emit_byte(parser, OP_PRINT);
}

static void expression_statement(ms_parser_t* parser) {
    expression(parser);
    consume(parser, TOKEN_NEWLINE, "Expect newline after expression.");
    emit_byte(parser, OP_POP);
}

static void statement(ms_parser_t* parser) {
    if (match(parser, TOKEN_PRINT)) {
        print_statement(parser);
    } else {
        expression_statement(parser);
    }
}

static void declaration(ms_parser_t* parser) {
    statement(parser);
    
    if (parser->panic_mode) {
        // 同步到下一个语句
        while (parser->current.type != TOKEN_EOF) {
            if (parser->previous.type == TOKEN_NEWLINE) return;
            
            switch (parser->current.type) {
                case TOKEN_CLASS:
                case TOKEN_FUNC:
                case TOKEN_VAR:
                case TOKEN_FOR:
                case TOKEN_IF:
                case TOKEN_WHILE:
                case TOKEN_PRINT:
                case TOKEN_RETURN:
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
}

bool ms_compile(const char* source, ms_chunk_t* chunk) {
    ms_lexer_t lexer;
    ms_lexer_init(&lexer, source);
    
    ms_parser_t parser;
    ms_parser_init(&parser, &lexer);
    parser.compiling_chunk = chunk;
    
    advance(&parser);
    
    while (!match(&parser, TOKEN_EOF)) {
        declaration(&parser);
    }
    
    end_compiler(&parser);
    return !parser.had_error;
}