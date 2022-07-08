#include "freestanding.h"
#include "log/log.h"

#define NUM_CAPABILITIES 64

enum Capabilities {
    CAP_NONE = 0,
    CAP_FRAMEBUFFER = 1 << 0,
    CAP_PHYSICAL_MEMORY = 1 << 1,
    CAP_SERIAL = 1 << 2,
};

enum Request_answer {
    REQUEST_ANSWER_OK = 0, // If no one use capability
    REQUEST_ANSWER_TAKEN_EX = 1, // If someone use capability exclusively
    REQUEST_ANSWER_TAKEN_SHARED = 2, // If someone use capability in shared mode, providing an uring
    REQUEST_ANSWER_TAKEN_BY_CUR = 2, // If someone use capability in shared mode, providing an uring
};

typedef struct
{
    uint64_t id;
    uint64_t owner;
    bool is_shared;
} capability_desc;

static capability_desc caps_handler[NUM_CAPABILITIES] = {0};

/*
void init_capabilities()
{
    for (int i = 0; i < NUM_CAPABILITIES; i++)
    {
        caps_handler[i].id = i;
        caps_handler[i].owner = REQUEST_ANSWER_OK;
        caps_handler[i].is_shared = false;
    }
    LOG_OK("Capabilities initialized");
}*/

uint8_t check(uint8_t capability, uint32_t inst)
{
    capability_desc *cap = &caps_handler[capability];
    if(capability != cap->id)
    {
        for(size_t i = 0; i < NUM_CAPABILITIES; i++)
        {
            if(caps_handler[i].id == capability)
            {
                cap = caps_handler + i;
                break;
            }
        }
    }
    uint8_t owner = cap->owner;
    if(owner == 0)
        return REQUEST_ANSWER_OK;
    if(owner == inst)
        return REQUEST_ANSWER_TAKEN_BY_CUR;
    if(cap->is_shared)
        return REQUEST_ANSWER_TAKEN_SHARED;
    return REQUEST_ANSWER_TAKEN_EX;
}

uint8_t request(uint8_t capability, uint32_t inst, uint8_t is_exclusive)
{

    uint8_t answer = check(capability, inst);
    if (answer == REQUEST_ANSWER_OK)
    {
        caps_handler[capability].owner = inst;
        caps_handler[capability].is_shared = is_exclusive;
    LOG_INFO("Requesting capability {d}", capability);
        return 1;
    }
    else if (answer == REQUEST_ANSWER_TAKEN_BY_CUR)
    {
        caps_handler[capability].is_shared = is_exclusive;
        return 1;
    }
    return 0;
}
