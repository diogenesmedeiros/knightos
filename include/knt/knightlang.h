#ifndef KNT_KNIGHTLANG_H
#define KNT_KNIGHTLANG_H

typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_ASSIGN,
    TOKEN_FUNC,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_COMMA,
    TOKEN_OPERATOR,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[64];
} Token;

typedef enum {
    STMT_VAR_ASSIGN,
    STMT_PRINT,
    STMT_FUNC_DECL,
    STMT_FUNC_CALL
} StatementType;

typedef struct Statement {
    StatementType type;
    char name[64];
    char value[256];
    
    struct Statement* body; 
    struct Statement* next;
} Statement;

void run_knightlang_script(const char* code);
void run_knightlang_file(const char* filename);

Token peek_token();
Token advance_token();
void init_lexer(const char* src);

#endif
