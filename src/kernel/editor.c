#include <fs/file.h>
#include <fs/fs.h>
#include <kernel/terminal.h>
#include <kernel/disk.h>
#include <lib/string.h>
#include <lib/stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_LINES 100
#define MAX_LINE_LEN 128
#define SCREEN_ROWS 25
#define SCREEN_COLS 80

static char lines[MAX_LINES][MAX_LINE_LEN];
static int line_count = 1;
static int cursor_line = 0;
static int cursor_col = 0;

extern void terminal_clear();
extern void terminal_print(const char* str);
extern void terminal_putc(char c);
extern void update_cursor(int row, int col);

extern void keyboard_poll();
extern bool keyboard_has_char();
extern char keyboard_get_char();

static void print_line_with_padding(const char* str) {
    int len = strlen(str);
    terminal_print(str);
    for (int i = len; i < SCREEN_COLS; i++) {
        terminal_putc(' ');
    }
    terminal_putc('\n');
}

static void print_line_of_chars(char c) {
    for (int i = 0; i < SCREEN_COLS; i++) {
        terminal_putc(c);
    }
    terminal_putc('\n');
}

static void editor_print_screen(const char* filename) {
    terminal_clear();

    const char* header = " KnightEditor v1.0 - ";
    terminal_print(header);
    terminal_print(filename);
    int header_len = strlen(header) + strlen(filename);
    for (int i = header_len; i < SCREEN_COLS; i++) {
        terminal_putc(' ');
    }
    terminal_putc('\n');

    print_line_of_chars('-');

    int max_visible_lines = SCREEN_ROWS - 4;

    for (int i = 0; i < max_visible_lines; i++) {
        if (i < line_count) {
            print_line_with_padding(lines[i]);
        } else {
            for (int j = 0; j < SCREEN_COLS; j++) {
                terminal_putc(' ');
            }
            terminal_putc('\n');
        }
    }

    print_line_of_chars('-');

    const char* footer = " Ctrl+X = sair | Enter = nova linha | Backspace = apagar ";
    print_line_with_padding(footer);

    update_cursor(cursor_line + 2, cursor_col);
}

static void editor_insert_char(char c) {
    if (cursor_line >= MAX_LINES) return;
    int len = strlen(lines[cursor_line]);
    if (len >= MAX_LINE_LEN - 1) return;
    if (cursor_col > len) cursor_col = len;

    for (int i = len; i >= cursor_col; i--) {
        lines[cursor_line][i + 1] = lines[cursor_line][i];
    }
    lines[cursor_line][cursor_col] = c;
    cursor_col++;
}

static void editor_new_line() {
    if (line_count >= MAX_LINES) return;
    int len = strlen(lines[cursor_line]);
    if (cursor_col > len) cursor_col = len;

    for (int i = line_count; i > cursor_line; i--) {
        strcpy(lines[i], lines[i - 1]);
    }
    line_count++;

    strcpy(lines[cursor_line + 1], &lines[cursor_line][cursor_col]);
    lines[cursor_line][cursor_col] = '\0';

    cursor_line++;
    cursor_col = 0;
}

static void editor_backspace() {
    if (cursor_line == 0 && cursor_col == 0) return;

    if (cursor_col > 0) {
        int len = strlen(lines[cursor_line]);
        for (int i = cursor_col - 1; i < len; i++) {
            lines[cursor_line][i] = lines[cursor_line][i + 1];
        }
        cursor_col--;
    } else {
        int prev_len = strlen(lines[cursor_line - 1]);
        int cur_len = strlen(lines[cursor_line]);

        if (prev_len + cur_len < MAX_LINE_LEN) {
            strcat(lines[cursor_line - 1], lines[cursor_line]);
            for (int i = cursor_line; i < line_count - 1; i++) {
                strcpy(lines[i], lines[i + 1]);
            }
            line_count--;
            cursor_line--;
            cursor_col = prev_len;
        }
    }
}

static void editor_move_cursor_up() {
    if (cursor_line > 0) {
        cursor_line--;
        int len = strlen(lines[cursor_line]);
        if (cursor_col > len) cursor_col = len;
    }
}

static void editor_move_cursor_down() {
    if (cursor_line < line_count - 1) {
        cursor_line++;
        int len = strlen(lines[cursor_line]);
        if (cursor_col > len) cursor_col = len;
    }
}

static void editor_move_cursor_left() {
    if (cursor_col > 0) {
        cursor_col--;
    } else if (cursor_line > 0) {
        cursor_line--;
        cursor_col = strlen(lines[cursor_line]);
    }
}

static void editor_move_cursor_right() {
    int len = strlen(lines[cursor_line]);
    if (cursor_col < len) {
        cursor_col++;
    } else if (cursor_line < line_count - 1) {
        cursor_line++;
        cursor_col = 0;
    }
}

void editor_save(const char* filename) {
    int sector = fs_find(filename);
    if (sector < 0) {
        terminal_print("Arquivo nao encontrado para salvar: ");
        terminal_print(filename);
        terminal_print("\n");
        return;
    }

    char buffer[512];
    memset(buffer, 0, sizeof(buffer));

    int pos = 0;
    for (int i = 0; i < line_count; i++) {
        int len = strlen(lines[i]);
        if (pos + len + 1 > 512) break;
        memcpy(&buffer[pos], lines[i], len);
        pos += len;
        buffer[pos++] = '\n';
    }

    ata_write_sector(sector, (uint8_t*)buffer);
    terminal_print("Arquivo salvo.\n");
}

void editor_open(const char* filename) {
    for (int i = 0; i < MAX_LINES; i++) {
        lines[i][0] = '\0';
    }

    char* content = read_file(filename);
    if (!content) return;

    int i = 0;
    line_count = 0;
    cursor_line = 0;
    cursor_col = 0;

    while (content[i] && line_count < MAX_LINES) {
        int len = 0;
        while (content[i] && content[i] != '\n' && len < MAX_LINE_LEN - 1) {
            lines[line_count][len++] = content[i++];
        }
        lines[line_count][len] = '\0';
        line_count++;
        if (content[i] == '\n') i++;
    }

    if (line_count == 0) line_count = 1;

    free(content);
}


void editor_run(const char* filename) {
    editor_open(filename);
    editor_print_screen(filename);

    while (1) {
        keyboard_poll();

        if (!keyboard_has_char()) continue;

        char c = keyboard_get_char();
        bool dirty = false;

        if (c == 24) {
            terminal_clear();
            editor_save(filename);
            terminal_print("Saindo do editor...\n");
            break;
        } else if (c == '\r' || c == '\n') {
            editor_new_line();
            dirty = true;
        } else if (c == 8 || c == 127) {
            editor_backspace();
            dirty = true;
        } else if (c >= 32 && c <= 126) {
            editor_insert_char(c);
            dirty = true;
        }

        if (dirty) editor_print_screen(filename);
    }
}
