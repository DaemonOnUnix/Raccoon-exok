#include <stdint.h>
#include <stddef.h>

#include "ABI/ABI.h"

// static volatile char test_string[] = "Hello world!";
// static volatile char *rodata = "Hello world!";

enum map_flags {

    MAP_PRESENT = 1,
    MAP_WRITE = 2,
    MAP_USER = 4,
    MAP_COPY_ON_WRITE = 8,
    MAP_EXECUTE = 16,
    MAP_SWAPPED = 32,
};

void *mmap(uintptr_t addr, size_t size, int flags);
int get_capability(int capability_id, uint8_t is_exclusive);

uint64_t sign_extend_48(uint64_t value) {
    uint64_t sign_bit = (value >> 47) & 1;
    return (value & 0xFFFFFFFFFFFF) | (sign_bit << 47);
}

uint64_t craft_addr(uint64_t offset_l4, uint64_t offset_l3, uint64_t offset_l2, uint64_t offset_l1, uint64_t offset_l0){
    return sign_extend_48(offset_l0 | (offset_l1 << 12 ) | (offset_l2 << 21) | (offset_l3 << 30 ) | (offset_l4 << 39));
}
__attribute__((section(".entry"), used))
void _main()
{
    asm volatile("push rbp");
    asm volatile("mov rbp, rsp");
    uintptr_t recursive_mapping_entry = 0;
    int val = get_capability(CAP_SERIAL, 1);
    while(1);

    // uintptr_t addr = craft_addr(, 0, 0, 0, 0);
    //volatile char *plouf = mmap(0x780000, 0x1000, MAP_PRESENT | MAP_WRITE | MAP_USER);
    //*plouf = 5;
    //while(1);
}

int get_capability(int capability_id, uint8_t is_exclusive)
{
    asm volatile("mov r15, 0x2");
    asm volatile("mov rdi, %0" : : "r"((uint64_t)capability_id));
    asm volatile("mov rsi, %0" : : "r"((uint64_t)is_exclusive));
    int ret;
    asm volatile("syscall" : "=a"(ret):);
    return ret;
}

void *mmap(uintptr_t addr, size_t size, int flags)
{
    asm volatile("mov r15, 1");
    asm volatile("mov rdi, %0" : : "r"(addr));
    asm volatile("mov rsi, %0" : : "r"(size));
    asm volatile("mov r8, %0" : : "r"((uint64_t)flags));
    void *ret;
    asm volatile("syscall" : "=a"(ret):);
    return ret;
}

int plouf(int a, int b){
    return a + b;
}