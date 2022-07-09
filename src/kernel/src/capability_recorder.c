#include "cap_mgr.h"
#include "init.h"
#include "interface_struct/interface_struct.h"

static struct capability caps[MAX_CAPABILITIES];

// TODO: clean
static int32_t second_strcmp(volatile const char *a, volatile const char *b) {
    while (*a && *b && *a == *b) {
        a++;
        b++;
    }
    return *a - *b;
}

void init_capabilities_record(interface_struct *interface, const char *current_system) {

    init_capabilities(caps, MAX_CAPABILITIES, current_system);

    for (size_t i = 0; i < MAX_CAPABILITIES; i++) {
        if(!second_strcmp(caps[i].name, "framebuffer")) {
            caps[i].begin_addr = interface->framebuffer.begin;
            caps[i].end_addr = interface->framebuffer.end;
        }
        caps[i].core_number = -1;
    }
}

char get_capability(const char *name, int16_t core_number)
{
    for (size_t i = 0; i < MAX_CAPABILITIES; i++) {
        if(!second_strcmp(caps[i].name, name)) {
            if(caps[i].core_number == -1) {
                caps[i].core_number = core_number;
                return 1;
            }
            return 0;
        }
    }
    return 0;
}

char release_capability(const char *name, int16_t core_number)
{
    for (size_t i = 0; i < MAX_CAPABILITIES; i++) {
        if(!second_strcmp(caps[i].name, name)) {
            if(caps[i].core_number == core_number) {
                caps[i].core_number = -1;
                return 1;
            }
            return 0;
        }
    }
    return 0;
}
