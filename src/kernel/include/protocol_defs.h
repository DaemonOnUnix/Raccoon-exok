#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

enum IOpermvalues {
    IO_NONE = 0,
    IO_READ = 1,
    IO_WRITE = 2,
    IO_READ_WRITE = 3,
};

struct capability {
    char *name;
    uintptr_t begin_addr;
    uintptr_t end_addr;
    char IOperms;
    int16_t core_number;
};

struct protocol
{
    struct capability *capabilities_copy;
    uint64_t page_size;
    uint64_t stack_size;
    uint32_t recursive_mapping_entry;
    uint32_t num_capabilities;
    uint16_t core_number;
    uint16_t num_core_has_libOS;
};

#endif