#include <kernel/terminal.h>
#include <commands/command.h>
#include <fs/journal.h>

void cmd_journal(const char* _) {
    (void)_;

    struct journal_entry entries[MAX_JOURNAL_ENTRIES];
    ata_read_journal(entries);

    terminal_print("Journal entries:\n");

    for (int i = 0; i < MAX_JOURNAL_ENTRIES; i++) {
        struct journal_entry* e = &entries[i];

        if (e->type != 0) {
            terminal_print("Entry ");
            char idx[4];
            itoa(i, idx, 10);
            terminal_print(idx);
            terminal_print(": ");

            switch (e->type) {
                case JOURNAL_CREATE_FILE: terminal_print("CREATE_FILE "); break;
                case JOURNAL_MKDIR: terminal_print("MKDIR "); break;
                case JOURNAL_RM: terminal_print("RM "); break;
                default: terminal_print("UNKNOWN "); break;
            }

            terminal_print(e->name);
            terminal_print(" | parent sector: ");
            itoa(e->parent_sector, idx, 10);
            terminal_print(idx);
            terminal_print(" | target sector: ");
            itoa(e->target_sector, idx, 10);
            terminal_print(idx);
            terminal_print(" | committed: ");
            terminal_print(e->committed ? "yes" : "no");
            terminal_putc('\n');
        }
    }
}