#include <knt/parser.h>
#include <lib/stdlib.h>
#include <lib/string.h>

static Statement* statements = NULL;
static Statement* last_stmt = NULL;

void add_statement(Statement* s) {
    if (!statements) statements = s;
    else last_stmt->next = s;
    last_stmt = s;
}

void parse_script(const char* src) {
    statements = NULL;
    last_stmt = NULL;
    init_lexer(src);

    while (1) {
        Token t = peek_token();
        if (t.type == TOKEN_EOF) break;

        if (t.type == TOKEN_FUNC) {
            advance_token(); // consome 'func'
            Token name = advance_token(); // nome da função
            advance_token(); // consome (
            advance_token(); // consome )
            advance_token(); // consome {
            Statement* body = parse_block();
            advance_token(); // consome }

            Statement* s = malloc(sizeof(Statement));
            s->type = STMT_FUNC_DECL;
            strcpy(s->name, name.lexeme);
            s->body = body;
            s->next = NULL;
            add_statement(s);
        }

        else if (t.type == TOKEN_IDENTIFIER) {
            Token name = advance_token();

            if (peek_token().type == TOKEN_ASSIGN) {
                advance_token(); // =
                Token val = advance_token();

                Statement* s = malloc(sizeof(Statement));
                s->type = STMT_VAR_ASSIGN;
                strcpy(s->name, name.lexeme);
                strcpy(s->value, val.lexeme);
                s->next = NULL;
                add_statement(s);
            } else if (peek_token().type == TOKEN_LPAREN) {
                advance_token(); // (
                Token val = advance_token(); // argumento
                advance_token(); // )

                Statement* s = malloc(sizeof(Statement));
                if (strcmp(name.lexeme, "print") == 0) {
                    s->type = STMT_PRINT;
                    strcpy(s->value, val.lexeme);
                } else {
                    s->type = STMT_FUNC_CALL;
                    strcpy(s->name, name.lexeme);
                }
                s->next = NULL;
                add_statement(s);
            }
        } else {
            advance_token(); // ignora token desconhecido
        }
    }
}

Statement* get_statements() {
    return statements;
}

Statement* parse_block() {
    Statement* head = NULL;
    Statement* tail = NULL;

    while (peek_token().type != TOKEN_RBRACE && peek_token().type != TOKEN_EOF) {
        Token t = peek_token();
        if (t.type == TOKEN_IDENTIFIER) {
            Token name = advance_token();
            if (peek_token().type == TOKEN_ASSIGN) {
                advance_token(); // =
                Token val = advance_token();
                Statement* s = malloc(sizeof(Statement));
                s->type = STMT_VAR_ASSIGN;
                strcpy(s->name, name.lexeme);
                strcpy(s->value, val.lexeme);
                s->next = NULL;

                if (!head) head = tail = s;
                else tail = tail->next = s;
            } else if (peek_token().type == TOKEN_LPAREN) {
                advance_token(); // (
                Token val = advance_token(); // valor dentro do print
                advance_token(); // )

                Statement* s = malloc(sizeof(Statement));
                if (strcmp(name.lexeme, "print") == 0) {
                    s->type = STMT_PRINT;
                    strcpy(s->value, val.lexeme);
                } else {
                    s->type = STMT_FUNC_CALL;
                    strcpy(s->name, name.lexeme);
                }
                s->next = NULL;

                if (!head) head = tail = s;
                else tail = tail->next = s;
            }
        } else {
            advance_token(); // ignorar
        }
    }

    return head;
}