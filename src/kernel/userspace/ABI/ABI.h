#ifndef EXO_EXPERIMENT_STRUCT_H
#define EXO_EXPERIMENT_STRUCT_H

#include <stdint.h>

#define __packed __attribute__((packed))

enum Request_answer {
    REQUEST_ANSWER_OK = 0, // If no one use capability
    REQUEST_ANSWER_TAKEN_EX = 1, // If someone use capability exclusively
    REQUEST_ANSWER_TAKEN_SHARED = 2, // If someone use capability in shared mode, providing an uring
};

enum Capabilities {
    CAP_NONE = 0,
    CAP_FRAMEBUFFER = 1 << 0,
    CAP_PHYSICAL_MEMORY = 1 << 1,
    CAP_SERIAL = 1 << 2,
};

struct Capability {
    enum Capabilities capability;
    uint8_t data[120];
};

struct Range {
    uint32_t start;
    uint32_t end;
};

struct System_data {
    void (*puts)(const char *); // For debugging purposes
    struct Range shared_memory[10]; // Ranges of guaranteed shared memory for urings
};

#endif