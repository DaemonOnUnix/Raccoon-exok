#include "arch/arch.h"
#include "arch/archtypes.h"
#include <stdint.h>

static register_t pmaps[MAX_CORE] = {0};
static register_t entries[MAX_CORE] = {0};

void store_pmap(register_t pmap, int core){
    pmaps[core] = pmap;
}

register_t load_pmap(int core){
    return pmaps[core];
}

void store_entry(register_t entry, int core){
    entries[core] = entry;
}

register_t load_entry(int core){
    return entries[core];
}
