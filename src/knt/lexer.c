#include <lib/string.h>
#include <knt/lexer.h>
#include <lib/libc_compat.h>
#include <lib/stdlib.h>

static const char* source;
static int pos;
static Token current_token;

Token peek_token() {
    return current_token;
}

Token advance_token() {
    Token t = current_token;
    current_token = next_token();
    return t;
}

Token next_token() {
    while (isspace(source[pos])) pos++;

    if (source[pos] == '\0') return (Token){ TOKEN_EOF, {0} };

    if (isalpha(source[pos])) {
        int start = pos;
        while (isalnum(source[pos])) pos++;
        Token t = {0};
        t.type = TOKEN_IDENTIFIER;
        strncpy(t.lexeme, source + start, pos - start);
        t.lexeme[pos - start] = '\0';

        if (strcmp(t.lexeme, "func") == 0) t.type = TOKEN_FUNC;
        return t;
    }

    if (isdigit(source[pos])) {
        int start = pos;
        while (isdigit(source[pos])) pos++;
        Token t = {0};
        t.type = TOKEN_NUMBER;
        strncpy(t.lexeme, source + start, pos - start);
        t.lexeme[pos - start] = '\0';
        return t;
    }

    if (source[pos] == '"') {
        pos++;
        int start = pos;
        while (source[pos] != '"' && source[pos] != '\0') pos++;
        Token t = {0};
        t.type = TOKEN_STRING;
        strncpy(t.lexeme, source + start, pos - start);
        t.lexeme[pos - start] = '\0';
        if (source[pos] == '"') pos++;
        return t;
    }

    Token t = {0};
    char c = source[pos++];
    switch (c) {
        case '=': t.type = TOKEN_ASSIGN; break;
        case '(': t.type = TOKEN_LPAREN; break;
        case ')': t.type = TOKEN_RPAREN; break;
        case '{': t.type = TOKEN_LBRACE; break;
        case '}': t.type = TOKEN_RBRACE; break;
        case ',': t.type = TOKEN_COMMA; break;
        case '+': case '-': case '*': case '/':
            t.type = TOKEN_OPERATOR; t.lexeme[0] = c; t.lexeme[1] = '\0'; break;
        default: t.type = TOKEN_EOF; break;
    }
    return t;
}

void init_lexer(const char* src) {
    source = src;
    pos = 0;
    current_token = next_token();
}