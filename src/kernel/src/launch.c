#include <stdint.h>
#include "log/log.h"
#include "memory/vmmwrapper.h"
#include "utils/functions.h"
#include "init/initfs.h"
#include "init.h"

#define ELF_HEADER_MAGIC "\177ELF"
#define PACKED __attribute__((packed))


enum elf_class
{
    ELF_CLASS_INVALID = 0,
    ELF_CLASS_32 = 1,
    ELF_CLASS_64 = 2,
};

enum elf_data_encoding
{
    ELF_ENCODING_INVALID = 0,
    ELF_ENCODING_LITTLE_ENDIAN = 1,
    ELF_ENCODING_BIG_ENDIAN = 2,
};

enum elf_type
{
    ELF_TYPE_NONE = 0,
    ELF_TYPE_RELOCATABLE = 1,
    ELF_TYPE_EXECUTABLE = 2,
    ELF_TYPE_DYNAMIC = 3,
    ELF_TYPE_CORE = 4,
};

enum elf_program_header_types
{
    ELF_PROGRAM_HEADER_NULL = 0,
    ELF_PROGRAM_HEADER_LOAD = 1,
    ELF_PROGRAM_HEADER_DYNAMIC = 2,
    ELF_PROGRAM_HEADER_INTERPRET = 3,
    ELF_PROGRAM_HEADER_NOTE = 4,
};

enum elf_program_header_flags
{
    ELF_PROGRAM_HEADER_EXECUTABLE = 1 << 0,
    ELF_PROGRAM_HEADER_WRITABLE = 1 << 1,
    ELF_PROGRAM_HEADER_READABLE = 1 << 2,
};

typedef struct PACKED
{
    uint8_t magics[4];
    uint8_t elf_class;
    uint8_t data_encoding;
    uint8_t version;
    uint8_t os;
    uint8_t abi_version;
    uint8_t _padding[7];
} Elf64Ident;

typedef struct PACKED
{
    Elf64Ident ident;

    uint16_t object_type;
    uint16_t machine_type;
    uint32_t object_version;

    uint64_t entry;

    uint64_t programs_offset;
    uint64_t sections_offset;

    uint32_t flags;

    uint16_t size;

    uint16_t programs_size;
    uint16_t programs_count;

    uint16_t sections_size;
    uint16_t sections_count;

    uint16_t strings_section_index;
} Elf64Header;

typedef struct PACKED
{
    uint32_t type;  // elf_program_header_types
    uint32_t flags; // elf_program_header_flags

    uint64_t file_offset;
    uint64_t virtual_address;
    uint64_t physical_address;

    uint64_t file_size;
    uint64_t memory_size;

    uint64_t alignment;
} Elf64ProgramHeader;

typedef struct PACKED
{
    uint32_t name;
    uint32_t type;
    uint64_t flags;

    uint64_t virtual_address;
    uint64_t file_offset;
    uint64_t file_size;
    uint32_t link;
    uint32_t info;
    uint64_t addralign;
    uint64_t entry_size;
} Elf64SectionHeader;

char check_elf(Elf64Header *hdr)
{
    *((char*)hdr->ident.magics + 5) = 0;
    LOG_INFO("ELF magic: {s}", hdr->ident.magics);

    for(uint8_t *i = hdr->ident.magics; i < hdr->ident.magics + 4; i++)
        if(*i != ELF_HEADER_MAGIC[(uint8_t*)i - hdr->ident.magics])
            return 0;

    if(hdr->ident.elf_class != ELF_CLASS_64)
        return 0;

    if(hdr->object_type != ELF_TYPE_EXECUTABLE)
        return 0;

    return 1;
}


void map_section(Elf64ProgramHeader *phdr, uintptr_t file_pos)
{
    if(phdr->type != ELF_PROGRAM_HEADER_LOAD)
        return;

    kmmap(phdr->virtual_address, phdr->memory_size, 7);
    memcpy((void *)phdr->virtual_address, (void *)(phdr->file_offset + file_pos), phdr->file_size);

}

void map_shdr(Elf64SectionHeader *shdr, uintptr_t file_pos)
{
    if(shdr->type != ELF_PROGRAM_HEADER_LOAD)
        return;

    kmmap(shdr->virtual_address, shdr->file_size, 7);
    memcpy((void *)shdr->virtual_address, (void *)(shdr->file_offset + file_pos), shdr->file_size);
}


void map_elf_64(Elf64Header *hdr)
{
    LOG_INFO("Mapping ELF at {x}", hdr);
    if(!check_elf(hdr))
        return;

    LOG_INFO("programs_count: {d}", hdr->programs_count);
    for(uint16_t i = 0; i < hdr->programs_count; i++)
    {
        LOG_INFO("Mapping section {d}", i);
        Elf64ProgramHeader *phdr = (Elf64ProgramHeader *)((uintptr_t)(hdr->programs_offset + i * hdr->programs_size) + (uintptr_t)hdr);
        map_section(phdr, (uintptr_t)hdr);
    }
}

void entry_launch(void)
{
    extern struct init parsed;
    LOG_INFO("{s}", parsed.programs[COREID]);
    struct file *f = get_file(parsed.programs[COREID]);
    Elf64Header *hdr = (Elf64Header *)(f->begin);
    map_elf_64(hdr);
    LOG_INFO("Mapping finished. Jumping to {x}", hdr->entry);

    kmmap(0x10000, 0x2000, 7);
    asm volatile("movq %0, %%rsp" : : "r"(0x11000ll));
    asm volatile ("mov %0, %%rcx" : : "r"(hdr->entry));
    asm volatile ("mov $0x002, %r11");
    asm volatile ("sysretq");

    ((void (*)())hdr->entry)();
    while(1);
}