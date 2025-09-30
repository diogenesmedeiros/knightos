#include <kernel/terminal.h>
#include <lib/string.h>
#include <lib/stdio.h>
#include <knt/interpreter.h>

const char *get_var(const char *name)
{
    for (int i = 0; i < var_count; i++)
    {
        if (strcmp(vars[i].name, name) == 0)
            return vars[i].value;
    }
    return NULL;
}

void register_function(const char *name, Statement *body)
{
    strcpy(functions[func_count].name, name);
    functions[func_count].body = body;
    func_count++;
}

Statement *find_function(const char *name)
{
    for (int i = 0; i < func_count; i++)
    {
        if (strcmp(functions[i].name, name) == 0)
        {
            return functions[i].body;
        }
    }
    return NULL;
}

void set_var(const char *name, const char *val)
{
    for (int i = 0; i < var_count; i++)
    {
        if (strcmp(vars[i].name, name) == 0)
        {
            strcpy(vars[i].value, val);
            return;
        }
    }
    strcpy(vars[var_count].name, name);
    strcpy(vars[var_count].value, val);
    var_count++;
}

void run_statements(Statement *stmt)
{
    Statement *cursor = stmt;

    while (cursor)
    {
        if (cursor->type == STMT_FUNC_DECL)
        {
            register_function(cursor->name, cursor->body);
        }
        cursor = cursor->next;
    }

    cursor = stmt;
    while (cursor)
    {
        switch (cursor->type)
        {
        case STMT_VAR_ASSIGN:
            set_var(cursor->name, cursor->value);
            break;

        case STMT_FUNC_CALL:
        {
            Statement *body = find_function(cursor->name);
            if (body)
            {
                run_statements(body);
            }
            else
            {
                terminal_print("Function not found: ");
                terminal_print(cursor->name);
                terminal_print("\n");
            }
            break;
        }

        case STMT_PRINT:
        {
            const char *val = cursor->value;

            int len = strlen(val);
            if (len >= 2 && val[0] == '"' && val[len - 1] == '"')
            {
                char buffer[128];
                int copy_len = len - 2;
                if (copy_len >= 128)
                    copy_len = 127;
                strncpy(buffer, val + 1, copy_len);
                buffer[copy_len] = '\0';
                terminal_print(buffer);
            }
            else if (val[0] >= '0' && val[0] <= '9')
            {
                terminal_print(val);
            }
            else
            {
                const char *var_val = get_var(val);
                if (var_val)
                {
                    terminal_print(var_val);
                }
                else
                {
                    terminal_print(val);
                }
            }

            terminal_print("\n");
            break;
        }

        default:
            break;
        }

        cursor = cursor->next;
    }
}