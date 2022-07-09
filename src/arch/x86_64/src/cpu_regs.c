#include <stddef.h>
#include <stdint.h>
#include "cpu_regs_interface.h"

#define MAX_REGISTERS 16

static char my_strcmp(const char* a, const char* b)
{
    while (*a && *b)
        if (*a++ != *b++)
            return 1;
    return *a != *b;
}

static struct pair_name_value table[MAX_CORE][MAX_REGISTERS] = {0};
static char is_init = 0;

void init_cpu_regs(void)
{
    if (is_init)
        return;

    is_init = 1;
    for (size_t i = 0; i < MAX_CORE; i++)
    {
        table[i][0].name = "idtr";
        table[i][1].name = "gdtr";
        table[i][2].name = "cr3";
        table[i][3].name = "rflags";
    }
}

char struct_cpu_regs_interface_set(const char *id, uint64_t value, uint16_t core)
{
    init_cpu_regs();
    for (size_t i = 0; i < MAX_CORE; i++)
    {
        if (!my_strcmp(table[core][i].name, id))
        {
            table[core][i].value = value;
            return 1;
        }
    }
    return 0;
}

char struct_cpu_regs_interface_get(const char *id, uint64_t *value, uint16_t core)
{
    init_cpu_regs();
    for (size_t i = 0; i < MAX_CORE; i++)
    {
        if (!my_strcmp(table[core][i].name, id))
        {
            *value = table[core][i].value;
            return 1;
        }
    }
    return 0;
}
