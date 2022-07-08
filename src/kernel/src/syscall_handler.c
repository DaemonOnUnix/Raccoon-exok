#include "syscall/syscall.h"

#include "vmm/vmm.h"
#include "memory/vmmwrapper.h"
#include "log/log.h"
#include "cap_mgr.h"


register_t syscall_handler(struct syscall_pack *pack)
{
    LOG_INFO("syscall_id: {x}\nfirst parameter: {x}\nsecond parameter: {x}\nthird parameter: {x}\n", pack->syscall_id, pack->arg1, pack->arg2, pack->arg3);
    switch (pack->syscall_id) {
        case 1:
            {
                LOG_INFO("Acquiring capability: {s}\n", pack->arg1);
                return get_capability(pack->arg1, COREID);
            }
        case 2:
            {
                LOG_INFO("Releasing capability: {s}\n", pack->arg1);
                return release_capability(pack->arg1, COREID);
            }
    }
    LOG_ERR("Unknown syscall: {x}\n", pack->syscall_id);
    return pack->arg1;
}