#ifndef KNT_PARSER_H
#define KNT_PARSER_H

#include <knt/knightlang.h>

void add_statement(Statement* s);
void parse_script(const char* src);
Statement* get_statements();
Statement* parse_block();

#endif 