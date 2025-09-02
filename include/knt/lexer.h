#ifndef KNT_LEXER_H
#define KNT_LEXER_H

#include <knt/knightlang.h>

Token peek_token();
Token advance_token();
Token next_token();
void init_lexer(const char* src);

#endif