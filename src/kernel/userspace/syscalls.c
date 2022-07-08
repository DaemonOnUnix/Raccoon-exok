int get_capability(int capability_id, uint8_t is_exclusive)
{
    asm volatile("mov r15, 0x2");
    asm volatile("mov rdi, %0" : : "r"((uint64_t)capability_id));
    asm volatile("mov rsi, %0" : : "r"((uint64_t)is_exclusive));
    int ret;
    asm volatile("syscall" : "=a"(ret):);
    return ret;
}

int probe_capability(int capability_id)
{
    asm volatile("mov r15, 0x3");
    asm volatile("mov rdi, %0" : : "r"((uint64_t)capability_id));
    int ret;
    asm volatile("syscall" : "=a"(ret):);
    return ret;
}

