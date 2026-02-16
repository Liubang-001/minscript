#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include <stdbool.h>

// Token类型
typedef enum {
    // 单字符token
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
    TOKEN_COLON, TOKEN_NEWLINE,

    // 一个或两个字符token
    TOKEN_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL,

    // 字面量
    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

    // 关键字
    TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
    TOKEN_FOR, TOKEN_FUNC, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
    TOKEN_PRINT, TOKEN_RETURN, TOKEN_TRUE, TOKEN_VAR,
    TOKEN_WHILE, TOKEN_BREAK, TOKEN_CONTINUE,
    TOKEN_IMPORT, TOKEN_AS, TOKEN_FROM,

    // 缩进相关
    TOKEN_INDENT, TOKEN_DEDENT,

    TOKEN_ERROR, TOKEN_EOF
} ms_token_type_t;

// Token结构
typedef struct {
    ms_token_type_t type;
    const char* start;
    int length;
    int line;
    int column;
} ms_token_t;

// 词法分析器结构
typedef struct {
    const char* start;
    const char* current;
    int line;
    int column;
    
    // 缩进栈
    int indent_stack[32];
    int indent_level;
    int pending_dedents;
    
    bool at_line_start;
} ms_lexer_t;

// 词法分析器API
void ms_lexer_init(ms_lexer_t* lexer, const char* source);
ms_token_t ms_lexer_scan_token(ms_lexer_t* lexer);

#endif // LEXER_H