#ifndef EXO_INIT_H
#define EXO_INIT_H

struct init {
    uint32_t required_cores;
    char *programs[MAX_CORE];
};

#define INIT_FILE_NAME "init"
struct init parse_init(const char *filename);

#endif