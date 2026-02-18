#include "lexer.h"
#include <string.h>
#include <ctype.h>

static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

static bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

static bool is_at_end(ms_lexer_t* lexer) {
    return *lexer->current == '\0';
}

static char advance(ms_lexer_t* lexer) {
    lexer->column++;
    return *lexer->current++;
}

static bool match(ms_lexer_t* lexer, char expected) {
    if (is_at_end(lexer)) return false;
    if (*lexer->current != expected) return false;
    lexer->current++;
    lexer->column++;
    return true;
}

static char peek(ms_lexer_t* lexer) {
    return *lexer->current;
}

static char peek_next(ms_lexer_t* lexer) {
    if (is_at_end(lexer)) return '\0';
    return lexer->current[1];
}

static ms_token_t make_token(ms_lexer_t* lexer, ms_token_type_t type) {
    ms_token_t token;
    token.type = type;
    token.start = lexer->start;
    token.length = (int)(lexer->current - lexer->start);
    token.line = lexer->line;
    token.column = lexer->column - token.length;
    return token;
}

static ms_token_t error_token(ms_lexer_t* lexer, const char* message) {
    ms_token_t token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = lexer->line;
    token.column = lexer->column;
    return token;
}

static void skip_whitespace(ms_lexer_t* lexer) {
    for (;;) {
        char c = peek(lexer);
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance(lexer);
                break;
            case '#':
                // 注释，跳过到行尾但不消费换行符
                while (peek(lexer) != '\n' && !is_at_end(lexer)) {
                    advance(lexer);
                }
                return;
            default:
                return;
        }
    }
}

static ms_token_t string_token(ms_lexer_t* lexer) {
    while (peek(lexer) != '"' && !is_at_end(lexer)) {
        if (peek(lexer) == '\n') {
            lexer->line++;
            lexer->column = 0;
        }
        advance(lexer);
    }

    if (is_at_end(lexer)) {
        return error_token(lexer, "Unterminated string.");
    }

    // 结束的引号
    advance(lexer);
    return make_token(lexer, TOKEN_STRING);
}

static ms_token_t number_token(ms_lexer_t* lexer) {
    while (is_digit(peek(lexer))) advance(lexer);

    // 小数部分
    if (peek(lexer) == '.' && is_digit(peek_next(lexer))) {
        advance(lexer); // 消费'.'
        while (is_digit(peek(lexer))) advance(lexer);
    }

    return make_token(lexer, TOKEN_NUMBER);
}

static ms_token_type_t check_keyword(const char* start, int length,
                                     const char* rest, ms_token_type_t type) {
    if (strlen(rest) == length && memcmp(start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

static ms_token_type_t identifier_type(ms_lexer_t* lexer) {
    switch (lexer->start[0]) {
        case 'a': 
            if (lexer->current - lexer->start == 2) {
                return check_keyword(lexer->start + 1, 1, "s", TOKEN_AS);
            }
            return check_keyword(lexer->start + 1, lexer->current - lexer->start - 1, "nd", TOKEN_AND);
        case 'b': return check_keyword(lexer->start + 1, lexer->current - lexer->start - 1, "reak", TOKEN_BREAK);
        case 'c': 
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'l': return check_keyword(lexer->start + 2, lexer->current - lexer->start - 2, "ass", TOKEN_CLASS);
                    case 'o': return check_keyword(lexer->start + 2, lexer->current - lexer->start - 2, "ntinue", TOKEN_CONTINUE);
                }
            }
            break;
        case 'd': return check_keyword(lexer->start + 1, lexer->current - lexer->start - 1, "ef", TOKEN_DEF);
        case 'e': 
            if (lexer->current - lexer->start > 1) {
                if (lexer->start[1] == 'l') {
                    if (lexer->current - lexer->start == 4 && 
                        lexer->start[2] == 'i' && lexer->start[3] == 'f') {
                        return TOKEN_ELIF;
                    }
                    if (lexer->current - lexer->start == 4 && 
                        lexer->start[2] == 's' && lexer->start[3] == 'e') {
                        return TOKEN_ELSE;
                    }
                }
            }
            return TOKEN_IDENTIFIER;
        case 'f':
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'a': return check_keyword(lexer->start + 2, lexer->current - lexer->start - 2, "lse", TOKEN_FALSE);
                    case 'o': return check_keyword(lexer->start + 2, lexer->current - lexer->start - 2, "r", TOKEN_FOR);
                    case 'r': return check_keyword(lexer->start + 2, lexer->current - lexer->start - 2, "om", TOKEN_FROM);
                }
            }
            break;
        case 'i':
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'f': return check_keyword(lexer->start + 2, lexer->current - lexer->start - 2, "", TOKEN_IF);
                    case 'm': return check_keyword(lexer->start + 2, lexer->current - lexer->start - 2, "port", TOKEN_IMPORT);
                    case 'n': return check_keyword(lexer->start + 2, lexer->current - lexer->start - 2, "", TOKEN_IN);
                    case 's': return check_keyword(lexer->start + 2, lexer->current - lexer->start - 2, "", TOKEN_IS);
                }
            }
            break;
        case 'n':
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'i': return check_keyword(lexer->start + 2, lexer->current - lexer->start - 2, "l", TOKEN_NIL);
                    case 'o': return check_keyword(lexer->start + 2, lexer->current - lexer->start - 2, "t", TOKEN_NOT);
                }
            }
            break;
        case 'o': return check_keyword(lexer->start + 1, lexer->current - lexer->start - 1, "r", TOKEN_OR);
        case 'p': return check_keyword(lexer->start + 1, lexer->current - lexer->start - 1, "ass", TOKEN_PASS);
        case 'r': return check_keyword(lexer->start + 1, lexer->current - lexer->start - 1, "eturn", TOKEN_RETURN);
        case 'F':
            if (lexer->current - lexer->start > 1) {
                return check_keyword(lexer->start + 1, lexer->current - lexer->start - 1, "alse", TOKEN_FALSE);
            }
            break;
        case 'N': return check_keyword(lexer->start + 1, lexer->current - lexer->start - 1, "one", TOKEN_NONE);
        case 'T': return check_keyword(lexer->start + 1, lexer->current - lexer->start - 1, "rue", TOKEN_TRUE);
        case 't': return check_keyword(lexer->start + 1, lexer->current - lexer->start - 1, "rue", TOKEN_TRUE);
        case 'v': return check_keyword(lexer->start + 1, lexer->current - lexer->start - 1, "ar", TOKEN_VAR);
        case 'w': 
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'h': return check_keyword(lexer->start + 2, lexer->current - lexer->start - 2, "ile", TOKEN_WHILE);
                    case 'i': return check_keyword(lexer->start + 2, lexer->current - lexer->start - 2, "th", TOKEN_WITH);
                }
            }
            break;
    }
    return TOKEN_IDENTIFIER;
}

static ms_token_t identifier_token(ms_lexer_t* lexer) {
    while (is_alpha(peek(lexer)) || is_digit(peek(lexer))) {
        advance(lexer);
    }
    
    // Check for f-string: f"..." or f'...'
    if ((lexer->current - lexer->start == 1) && 
        (lexer->start[0] == 'f' || lexer->start[0] == 'F')) {
        char next = peek(lexer);
        if (next == '"' || next == '\'') {
            char quote = next;
            advance(lexer);  // consume quote
            
            // Scan until closing quote
            while (peek(lexer) != quote && !is_at_end(lexer)) {
                if (peek(lexer) == '\n') {
                    lexer->line++;
                    lexer->column = 0;
                }
                advance(lexer);
            }
            
            if (is_at_end(lexer)) {
                return error_token(lexer, "Unterminated f-string.");
            }
            
            advance(lexer);  // consume closing quote
            return make_token(lexer, TOKEN_FSTRING);
        }
    }
    
    return make_token(lexer, identifier_type(lexer));
}

void ms_lexer_init(ms_lexer_t* lexer, const char* source) {
    lexer->start = source;
    lexer->current = source;
    lexer->line = 1;
    lexer->column = 1;
    lexer->indent_level = 0;
    lexer->pending_dedents = 0;
    lexer->at_line_start = true;
    lexer->indent_stack[0] = 0;
}

ms_token_t ms_lexer_scan_token(ms_lexer_t* lexer) {
    // 处理待处理的DEDENT token
    if (lexer->pending_dedents > 0) {
        lexer->pending_dedents--;
        lexer->start = lexer->current;
        return make_token(lexer, TOKEN_DEDENT);
    }

    // 处理行首的缩进
    if (lexer->at_line_start) {
        lexer->at_line_start = false;
        
        // 跳过所有空行和注释行
        while (true) {
            // 计算当前行的缩进
            int indent = 0;
            while (peek(lexer) == ' ' || peek(lexer) == '\t') {
                if (peek(lexer) == ' ') indent++;
                else indent += 4; // tab = 4 spaces
                advance(lexer);
            }
            
            lexer->start = lexer->current;
            
            // 检查是否是空行或注释行
            if (peek(lexer) == '\n') {
                // 空行，跳过
                advance(lexer);
                lexer->line++;
                lexer->column = 1;
                continue;  // 继续处理下一行
            } else if (peek(lexer) == '#') {
                // 注释行，跳过到行尾
                while (peek(lexer) != '\n' && !is_at_end(lexer)) {
                    advance(lexer);
                }
                if (peek(lexer) == '\n') {
                    advance(lexer);
                    lexer->line++;
                    lexer->column = 1;
                    continue;  // 继续处理下一行
                }
            } else if (is_at_end(lexer)) {
                return make_token(lexer, TOKEN_EOF);
            }
            
            // 不是空行或注释行，处理缩进
            int current_indent = lexer->indent_stack[lexer->indent_level];
            
            if (indent > current_indent) {
                // 增加缩进
                lexer->indent_level++;
                lexer->indent_stack[lexer->indent_level] = indent;
                return make_token(lexer, TOKEN_INDENT);
            } else if (indent < current_indent) {
                // 减少缩进
                while (lexer->indent_level > 0 && 
                       lexer->indent_stack[lexer->indent_level] > indent) {
                    lexer->indent_level--;
                    lexer->pending_dedents++;
                }
                
                if (lexer->indent_stack[lexer->indent_level] != indent) {
                    return error_token(lexer, "Indentation error.");
                }
                
                if (lexer->pending_dedents > 0) {
                    lexer->pending_dedents--;
                    return make_token(lexer, TOKEN_DEDENT);
                }
            }
            
            // 缩进级别相同，继续正常处理
            break;
        }
    }

    skip_whitespace(lexer);
    lexer->start = lexer->current;

    if (is_at_end(lexer)) {
        // 文件结束时生成所有待处理的DEDENT
        if (lexer->indent_level > 0) {
            lexer->indent_level--;
            return make_token(lexer, TOKEN_DEDENT);
        }
        return make_token(lexer, TOKEN_EOF);
    }

    char c = advance(lexer);

    if (is_alpha(c)) return identifier_token(lexer);
    if (is_digit(c)) return number_token(lexer);

    switch (c) {
        case '(': return make_token(lexer, TOKEN_LEFT_PAREN);
        case ')': return make_token(lexer, TOKEN_RIGHT_PAREN);
        case '{': return make_token(lexer, TOKEN_LEFT_BRACE);
        case '}': return make_token(lexer, TOKEN_RIGHT_BRACE);
        case '[': return make_token(lexer, TOKEN_LEFT_BRACKET);
        case ']': return make_token(lexer, TOKEN_RIGHT_BRACKET);
        case ',': return make_token(lexer, TOKEN_COMMA);
        case '.': return make_token(lexer, TOKEN_DOT);
        case '-': return make_token(lexer, TOKEN_MINUS);
        case '+': return make_token(lexer, TOKEN_PLUS);
        case ';': return make_token(lexer, TOKEN_SEMICOLON);
        case '/':
            return make_token(lexer, match(lexer, '/') ? TOKEN_SLASH_SLASH : TOKEN_SLASH);
        case '*':
            return make_token(lexer, match(lexer, '*') ? TOKEN_STAR_STAR : TOKEN_STAR);
        case '%': return make_token(lexer, TOKEN_PERCENT);
        case ':': return make_token(lexer, TOKEN_COLON);
        case '!':
            return make_token(lexer, match(lexer, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return make_token(lexer, match(lexer, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<':
            return make_token(lexer, match(lexer, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return make_token(lexer, match(lexer, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '"': return string_token(lexer);
        case '\n':
            lexer->line++;
            lexer->column = 1;
            lexer->at_line_start = true;
            return make_token(lexer, TOKEN_NEWLINE);
    }

    return error_token(lexer, "Unexpected character.");
}