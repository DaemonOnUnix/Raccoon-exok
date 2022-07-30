#include "memory/vmmwrapper.h"
#include <protocol_defs.h>
#include <protocol_init.h>
#include <vmm/vmm.h>
#include <log/log.h>

void init_protocol(void) {
    extern struct capability caps[MAX_CAPABILITIES];
    uint64_t size_capabilities = sizeof(caps);
    struct capability *copy = (void*)space_alloc(size_capabilities, MAP_PRESENT | MAP_USER);
    struct protocol *protocol = (void*)space_alloc(sizeof(struct protocol), MAP_PRESENT | MAP_USER);
    if(!(copy && protocol))
        PANIC("Could not allocate memory for protocol");
    memcpy(copy, caps, size_capabilities);
    (*protocol) = (struct protocol){
        .capabilities_copy = copy,
        .page_size = 0x1000, //TODO: get from arch
        .stack_size = 0x2000, //TODO: have a real stack, see main.c and launch.c
        .recursive_mapping_entry = RECURSIVE_MAPPING_ENTRY,
        .num_capabilities = MAX_CAPABILITIES,
        .core_number = COREID,
        .num_core_has_libOS = 0 //TODO: tell number of cores that has loaded this libOS
    };
    LOG_OK("Init protocol at {x} with capability map at {x} done successfully.", protocol, copy);
}