#ifndef CAP_MGR_H
#define CAP_MGR_H

#include <stddef.h>
#include <stdint.h>
#include "interface_struct/interface_struct.h"

#define INIT_CAPABILITY_FILE_NAME "capabilities"
#define INIT_SCRIPT "init"

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

void init_capabilities_from_file(struct capability* caps, size_t max, const char* current_system);
void init_capabilities_record(interface_struct *interface, const char *current_system);
char get_capability(const char *name, int16_t core_number);
char release_capability(const char *name, int16_t core_number);

#endif
