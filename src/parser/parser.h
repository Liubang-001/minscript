#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"
#include "../vm/vm.h"

// 解析器结构
typedef struct {
    ms_token_t current;
    ms_token_t previous;
    bool had_error;
    bool panic_mode;
    ms_lexer_t* lexer;
    ms_chunk_t* compiling_chunk;
} ms_parser_t;

// 优先级
typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // > >= < <=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_PRIMARY
} ms_precedence_t;

typedef void (*ms_parse_fn_t)(ms_parser_t* parser);

// 解析规则
typedef struct {
    ms_parse_fn_t prefix;
    ms_parse_fn_t infix;
    ms_precedence_t precedence;
} ms_parse_rule_t;

// 解析器API
void ms_parser_init(ms_parser_t* parser, ms_lexer_t* lexer);
bool ms_compile(const char* source, ms_chunk_t* chunk);

#endif // PARSER_H