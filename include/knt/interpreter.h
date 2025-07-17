#ifndef KNT_INTERPRETER_H
#define KNT_INTERPRETER_H

#include <knt/knightlang.h>

typedef struct {
    char name[64];
    char value[256];
} Variable;

typedef struct {
    char name[64];
    Statement* body;
} Function;

static Variable vars[64];
static int var_count = 0;

static Function functions[32];
static int func_count = 0;

const char* get_var(const char* name);
void register_function(const char* name, Statement* body);
Statement* find_function(const char* name);
void set_var(const char* name, const char* val);
void run_statements(Statement* stmt);

#endif 