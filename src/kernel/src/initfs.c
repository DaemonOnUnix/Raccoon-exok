#include "arch/arch.h"
#include "init/initfs.h"
#include "log/log.h"

static struct file initrd_files[MAX_FILES_IN_INITRD];
static size_t file_count = 0;

static void my_strncpy(volatile char *dest, volatile const char *src, size_t n)
{
    size_t i;
    for (i = 0; i < n-1 && src[i]; i++)
        dest[i] = src[i];
    dest[i] = 0;
}

void register_new_file(const char* name, uintptr_t begin, uintptr_t end)
{
    if (file_count >= MAX_FILES_IN_INITRD)
        PANIC("Too many files in initrd");

    initrd_files[file_count].begin = begin;
    initrd_files[file_count].end = end;
    initrd_files[file_count].cursor = 0;
    my_strncpy(initrd_files[file_count].name, name, MAX_FILENAME_SIZE);
    file_count++;
}

static char my_strcmp(const char* a, const char* b)
{
    while (*a && *b)
        if (*a++ != *b++)
            return 1;
    return *a != *b;
}

struct file *get_files()
{
    return initrd_files;
}

struct file *get_file(const char *name)
{
    for (size_t i = 0; i < file_count; i++)
    {
        if (!my_strcmp(initrd_files[i].name, name))
            return initrd_files + i;
    }
    return NULL;
}
