#include "log/log.h"
#include "init/initfs.h"
#include "utils/functions.h"
// file_format:
/*
CORES 4
1 pid0
2 HANG
3 HANG
4 HANG
*/

struct init {
    uint32_t required_cores;
    char *programs[MAX_CORE];
};

int strncmp(const char *a, const char *b, size_t n);

uint32_t atou(char *str, char **ptr)
{
    uint32_t to_return = 0;
    while ((*str >= '0') && (*str <= '9'))
        to_return = to_return * 10 + *str++ - '0';
    if (ptr)
        *ptr = str;
    return to_return;
}

char *get_next_line(struct file *file, char *ptr)
{
    while ((uintptr_t)ptr < file->end)
    {
        if (*ptr == 0)
            return ptr + 1;
        ptr++;
    }
    return NULL;
}

struct init parse_init(const char *filename)
{
    struct init to_return = {0, {0}};
    struct file *file = get_file(filename);
    if (!file)
        PANIC("No init file");

    for(char *ptr = (char*)(file->begin); (uintptr_t)ptr < (uintptr_t)(file->end); ptr++)
    {
        if (*ptr == '\n')
            *ptr = '\0';
    }

    char *ptr = (char*)(file->begin);
    if (!strncmp(ptr, "CORES ", 6))
    {
        ptr += 6;
        to_return.required_cores = atou(ptr, NULL);
    } else
        PANIC("No CORES section");

    ptr = get_next_line(file, ptr);
    while ((uintptr_t)ptr < file->end && ptr)
    {
        if(*ptr == '\0')
            ptr = get_next_line(file, ptr);
        else
        {
            uint32_t index = atou(ptr, &ptr);
            to_return.programs[index] = ptr + 1;
            LOG_INFO("Core {d} -> {s}", index, to_return.programs[index]);
            *ptr = '\0';
            ptr = get_next_line(file, ++ptr);
        }
    }
    return to_return;
}