#include "arch/arch.h"
#include "arch/archtypes.h"
#include "interface_struct/interface_struct.h"
#include "log/log.h"
#include "vmm/vmm.h"

#include "multicore/interrupt_lock.h"
#include "multicore/pmap_recorder.h"

#include "init/initfs.h"

#include "vmm/vmm.h"

#include "cap_mgr.h"
#include "init.h"
#include "launch.h"
#include <stddef.h>
#include <protocol_init.h>

struct init parsed;

void kernel_main(void* generic_structure) {
    
    interface_struct *interface = bootstrap_arch(generic_structure);
    
    lock_ints();
    
    LOG_OK("All work finished.");

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
                for(size_t y = 1; y < i; y++)
                {
                    if(!strcmp(parsed.programs[y], parsed.programs[i]))
                    {
                        register_t pmap = load_pmap(y);
                        // TODO: have arch-independent pause
                        while(!load_entry(y))
                            ;

                        enable_mapping(pmap);
                        init_protocol();

                        // TODO: have a real stack
                        kmmap(0x10000 + 0x3000 * COREID , 0x2000, 7);
                        asm volatile("movq %0, %%rsp" : : "r"(0x10000ll + 0x3000 * COREID + 0x1980));
                        asm volatile ("mov %0, %%rcx" : : "r"(load_entry(y)));
                        asm volatile ("mov $0x002, %r11");
                        asm volatile ("sysretq");

                        while(1);

                    }
                }
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
    {
        for(size_t y = 1; y < parsed.required_cores; y++)
        {
            if(!strcmp(parsed.programs[y], parsed.programs[COREID]))
            {
                register_t pmap = load_pmap(y);
                // TODO: have arch-independent pause
                while(!load_entry(y))
                    ;
                LOG_INFO("Program {s} is already running, jumping at {x} with stack at {x} of size {x}.", parsed.programs[y], entry_launch, 0x10000 + 0x3000 * COREID, 0x1980);

                enable_mapping(pmap);

                // TODO: have a real stack
                kmmap(0x10000 + 0x3000 * COREID, 0x2000, 7);
                asm volatile("movq %0, %%rsp" : : "r"(0x10000ll + 0x3000 * COREID + 0x1980));
                asm volatile ("mov %0, %%rcx" : : "r"(load_entry(y)));
                asm volatile ("mov $0x002, %r11");
                asm volatile ("sysretq");

                while(1);

            }
        }
        entry_launch();
    }

    halt();
}
