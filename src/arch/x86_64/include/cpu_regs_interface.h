#ifndef CPU_REGS_INTERFACE_H
#define CPU_REGS_INTERFACE_H

#include <stdint.h>

struct pair_name_value
{
    const char *name;
    uint64_t value;
};

char struct_cpu_regs_interface_set(const char *id, uint64_t  value, uint16_t core);
char struct_cpu_regs_interface_get(const char *id, uint64_t *value, uint16_t core);

#endif
