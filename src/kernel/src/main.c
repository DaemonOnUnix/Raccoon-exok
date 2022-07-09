#include "arch/arch.h"
#include "interface_struct/interface_struct.h"
#include "log/log.h"
#include "vmm/vmm.h"

#include "multicore/interrupt_lock.h"

#include "init/initfs.h"

#include "vmm/vmm.h"

#include "cap_mgr.h"
#include "init.h"
#include "launch.h"
#include <stddef.h>

struct init parsed;

void kernel_main(void* generic_structure) {
    
    interface_struct *interface = bootstrap_arch(generic_structure);
    
    lock_ints();
    
    LOG_OK("All work finished.");
    
    struct file first = *get_files();
    struct file second = *(get_files() + 3);

    parsed = parse_init(INIT_FILE_NAME);
    LOG_INFO("Parsed init: {x} cores", parsed.required_cores);

    if(parsed.required_cores > interface->core_number)
        PANIC("Required cores ({x}) > cores count ({x})", parsed.required_cores, interface->core_number);

    for(size_t i = 0; i < parsed.required_cores; i++)
    {
        if(strcmp(parsed.programs[i], "HANG"))
        {
            LOG_INFO("Launching init: {s} for core {d}.", parsed.programs[i], i);
            if(COREID == i)
            {
                LOG_INFO("Jumping later...");
            }
            else
            {
                LOG_INFO("Awakening core {d}..., jumping at {x}", i, entry_launch);
                interface->launching_addresses[i] = entry_launch;
                LOG_INFO("Written address {x}", interface->launching_addresses[i]);
            }
        } else {
            LOG_INFO("Core {d} marked hanging...", i);
        }
    }

    LOG_OK("All cores awoken.");
    if(strcmp(parsed.programs[COREID], "HANG"))
        entry_launch();

    halt();
}
