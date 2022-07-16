#ifndef PMAP_RECORDER_H
#define PMAP_RECORDER_H

#include "arch/archtypes.h"

void store_pmap(register_t pmap, int core);
register_t load_pmap(int core);

void store_entry(register_t entry, int core);
register_t load_entry(int core);

#endif
