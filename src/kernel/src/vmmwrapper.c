#include "memory/vmmwrapper.h"
#include "vmm/vmm.h"
#include "log/log.h"
#include <stdatomic.h>
#include "multicore/lock.h"


// should be defined in arch vmm
uint64_t convert_to_arch_flags(uintptr_t flags);

CREATE_LOCK(space_alloc);

uintptr_t space_alloc(size_t size, uintptr_t flags)
{
    BEGIN_BOTTLENECK(space_alloc);
    uint64_t address = search_available(craft_addr(0,0,0,1,0), size);
    LOG_INFO("Allocating {x} bytes at {x}", size, address);
    if(address == 0)
        return 0;
    kmmap(address, size, convert_to_arch_flags(flags));
    END_BOTTLENECK(space_alloc);
    return address;
}

void space_free(uintptr_t addr, size_t size)
{
    kmunmap(addr, size, MEM_TO_UPPER);
}