#include "arch/arch.h"
#include "interface_struct/interface_struct.h"
#include "log/log.h"
#include "tests/tests.h"
#include "vmm/vmm.h"

#include "multicore/interrupt_lock.h"

#include "init/initfs.h"
#include "tasking/tasking.h"

#include "vmm/vmm.h"

#include "cap_mgr.h"
#include "init.h"
#include "launch.h"
#include <stddef.h>

void hello()
{
    LOG_OK("Hello");
}

void launch_hell()
{
    lock_ints();
    LOG_OK("Launching hell");
    //enable_mapping(create_page_directory());
    //setup_context_frame();
    //map_pics();
    struct file first = *get_files();
    //LOG_PANIC("PLOUF");
    kmmap(0, first.end - first.begin, 7);
    memcpy(0, (void*)first.begin, first.end - first.begin);
    //LOG_OK("plouf");
    kmmap(0x10000, 0x2000, 7);
    asm volatile("movq rsp, %0" : : "r"(0x11000ll));
    asm volatile ("mov rcx, 0x1000");
    asm volatile ("mov r11, 0x002");
    asm volatile ("sysretq");
}

struct init parsed;

void kernel_main(void* generic_structure) {
    
    interface_struct *interface = bootstrap_arch(generic_structure);
    
    lock_ints();
    
    LOG_OK("All work finished.");
    
    struct file first = *get_files();
    struct file second = *(get_files() + 3);
    // LOG_INFO("Loading first elf: {s}", first.name);
    // LOG_INFO("Loading second file: {s}", second.name);

    // init_capabilities(caps, MAX_CAPABILITIES, "amd64");
    // LOG_INFO("Capabilities loaded");
    // LOG_INFO("First capability: {s}", caps[0].name);
    // LOG_INFO("First capability begin_addr: {x}", caps[0].begin_addr);
    // LOG_INFO("First capability end_addr: {x}", caps[0].end_addr);
    // LOG_INFO("First capability IOperms: {x}", caps[0].IOperms);

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
    LOG_INFO("All cores awoken.");
    if(strcmp(parsed.programs[COREID], "HANG"))
        entry_launch();

    halt();
}
