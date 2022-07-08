#include "init/initfs.h"
#include "utils/functions.h"
#include "log/log.h"
#include "cap_mgr.h"

struct file *get_capability_file()
{
    return get_file(INIT_CAPABILITY_FILE_NAME);
}

struct file *get_init_script_file()
{
    return get_file(INIT_SCRIPT);
}

void to_lines(struct file *file)
{
    char *ptr = (char*)file->begin;
    while ((uintptr_t)ptr < file->end)
    {
        if (*ptr == '\n')
            *ptr = 0;
        ptr++;
    }
}

struct pair {
    uintptr_t begin;
    uintptr_t end;
};

int strncmp(const char *a, const char *b, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        if (a[i] != b[i])
            return 1;
    }
    return 0;
}

size_t strlen(const char *str)
{
    size_t i = 0;
    while (str[i])
        i++;
    return i;
}

struct pair get_section(struct file *capability_file, const char *section_name)
{
    struct pair to_return = {0, 0};
    char *ptr = (char*)capability_file->begin;
    if(!strncmp(ptr, "BEGIN ", 6))
    {
        ptr += 6;
        if(!strncmp(ptr, section_name, strlen(section_name)))
        {
            ptr += strlen(section_name);
            while(*(ptr-1) != '\0')
                ptr++;
            if((uintptr_t)ptr >= capability_file->end)
                return to_return;

            to_return.begin = (uintptr_t)ptr;
            while((uintptr_t)ptr < capability_file->end)
            {
                if(!strncmp(ptr, "\0END", 4))
                {
                    to_return.end = (uintptr_t)ptr;
                    return to_return;
                }
                ptr++;
            }

            PANIC("No end of section");
        }
    } else {
        LOG_INFO("No begin of section");
    }
    return to_return;
}

uintptr_t strtoul(char *str, char **end, int base)
{
    uintptr_t to_return = 0;
    while(((*str >= '0' && *str <= '9') || (*str >= 'a' && *str <= 'f') || (*str >= 'A' && *str <= 'F')))
    {
        if(*str >= '0' && *str <= '9')
            to_return = to_return*base + (*str-'0');
        else if(*str >= 'a' && *str <= 'f')
            to_return = to_return*base + (*str-'a'+10);
        else if(*str >= 'A' && *str <= 'F')
            to_return = to_return*base + (*str-'A'+10);
        else
            break;
        str++;
    }
    *end = str;
    return to_return;
}

void fill_capabilities(struct file *capability_file, struct capability* caps, size_t max, const char* current_system)
{
    to_lines(capability_file);
    size_t current_index = 0;
    struct pair bounds = get_section(capability_file, current_system);
    // LOG_INFO("{x} {x}", bounds.begin, bounds.end);
    if(bounds.begin == bounds.end)
        PANIC("No section for current system");

    char *ptr = (char*)bounds.begin;

    while(ptr < (char*)bounds.end)
    {
        if(!strncmp(ptr, "CAPABILITY ", 10))
        {
            ptr += 10;
            if(current_index >= max)
                PANIC("Too many capabilities");
            caps[current_index].name = ptr+1;
            while(*ptr != '\0')
                ptr++;
            ptr++;
            // caps[current_index].begin_addr = capability_file->cursor;
            while(ptr < (char*)bounds.end)
            {
                if(!strncmp(ptr, "    ADDRESS ", 12))
                {
                    ptr += 12;
                    caps[current_index].begin_addr = strtoul(ptr, &ptr, 16);
                    char *str = ptr;

                    while(((*str >= '0' && *str <= '9') || (*str >= 'a' && *str <= 'f') || (*str >= 'A' && *str <= 'F')))
                        str++;
                    ptr = str;

                    while(*ptr == ' ' || *ptr == '\t' || *ptr == ':' || *ptr == '-')
                        ptr++;

                    // LOG_INFO("{s}", ptr);

                    caps[current_index].end_addr = strtoul(ptr, &ptr, 16);

                    while(((*str >= '0' && *str <= '9') || (*str >= 'a' && *str <= 'f') || (*str >= 'A' && *str <= 'F')))
                        str++;
                    ptr=str;


                }
                if(!strncmp(ptr, "    IOperms ", 12))
                {
                    ptr += 12;
                    if(!strncmp(ptr, "rw", 2) || !strncmp(ptr, "wr", 2))
                    {
                        caps[current_index].IOperms = IO_READ | IO_WRITE;
                        ptr += 2;
                    }
                    else if(!strncmp(ptr, "r", 1))
                    {
                        caps[current_index].IOperms = IO_READ;
                        ptr += 1;
                    }
                    else if(!strncmp(ptr, "w", 1))
                    {
                        caps[current_index].IOperms = IO_WRITE;
                        ptr += 1;
                    }
                    else
                        PANIC("Unknown IO permissions");

                    while(*ptr == ' ' || *ptr == '\t' || *ptr == ':' || *ptr == '-')
                        ptr++;
                    if(*ptr != '\0')
                        PANIC("Error, unrecognized token in IO permissions");

                }
                while(*ptr != '\0')
                        ptr++;
                    ptr++;
            }
        } else {
            ptr++;
        }
    }
}

void init_capabilities(struct capability* caps, size_t max, const char* current_system)
{
    struct file *capability_file = get_capability_file();
    if(!capability_file)
        PANIC("No capability file");
    fill_capabilities(capability_file, caps, max, current_system);
}