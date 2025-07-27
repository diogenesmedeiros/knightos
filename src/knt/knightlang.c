#include <knt/knightlang.h>
#include <lib/stdio.h>
#include <lib/stdlib.h>
#include <fs/file.h>
#include <knt/parser.h>
#include <knt/lexer.h>
#include <knt/interpreter.h>

void run_knightlang_script(const char* code) {
    parse_script(code);
    Statement* stmts = get_statements();
    run_statements(stmts);
}

void run_knightlang_file(const char* filename) {
    char* code = read_file(filename);
    if (code) {
        run_knightlang_script(code);
        free(code);
    } else {
        terminal_print("Error opening .knt script\n");
    }
}