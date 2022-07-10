#ifndef EXO_INIT_H
#define EXO_INIT_H

#include "cap_mgr.h"

struct init {
    uint32_t required_cores;
    char *programs[MAX_CORE];
};

#define INIT_FILE_NAME "init"
struct init parse_init(const char *filename);
void init_capabilities(struct capability* caps, size_t max, const char* current_system);

#endif