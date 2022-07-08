#ifndef INTERFACE_STRUCT_H
#define INTERFACE_STRUCT_H

#define MAX_CORES MAX_CORE

#include "freestanding.h"

struct interface_range {
    uintptr_t begin;
    uintptr_t end;
};

typedef void (*launching_address)();
typedef struct {
    unsigned char core_number;
    launching_address launching_addresses[MAX_CORE];
    struct interface_range framebuffer;
} interface_struct;

#endif