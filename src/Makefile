KERNEL := Raccoon

ARCH = x86_64
ARCH_FOLDER = arch
MAX_CORE = 32
MAX_CAPABILITIES = 64
KERNEL_CONFIG = -DMAX_CORE=${MAX_CORE} -DARCH=${ARCH} -DARCH_FOLDER=${ARCH_FOLDER} -DMAX_CAPABILITIES=${MAX_CAPABILITIES}

KERNEL_DIR = kernel

CC = gcc
LOGS = #-DAPIC_DEBUG  -DVMM_DEBUG -DPMM_DEBUG -DIDT_DEBUG -DGDT_DEBUG

CFLAGS = -Wall -Wextra -Ofast -DPTESTS -I$(ARCH_FOLDER)/$(ARCH)/include/ -I$(KERNEL_DIR)/include/ $(LOGS) -g $(KERNEL_CONFIG)

ASMPARAM = -f elf64 -F dwarf

BUILD = build

ISO_FILES = $(KERNEL).elf limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-eltorito-efi.bin init capabilities


LINK_SCRIPT = $(ARCH_FOLDER)/$(ARCH)/link.ld

INTERNALLDFLAGS :=     \
	-nostdlib      \
	-T$(LINK_SCRIPT)    \
	-z max-page-size=0x1000\
	#-Wl,-static,-pie,--no-dynamic-linker,-ztext -fno-pic -fpie

INTERNALCFLAGS  :=                  \
	-std=gnu17           \
	-ffreestanding       \
	-fno-stack-protector \
	-fno-pic -fno-pie    \
	-mno-red-zone		 \
	-mno-sse			 \
	-fno-zero-initialized-in-bss \
	-mcmodel=kernel		 \
	-fno-delete-null-pointer-checks \
	-m64

SRCDIRS := $(shell find ./$(ARCH_FOLDER)/$(ARCH) -type d)
SRCDIRS += $(shell find ./$(KERNEL_DIR) -type d)

CFILES := $(shell find ./$(ARCH_FOLDER)/$(ARCH) -type f -name '*.c')
SFILES := $(shell find ./$(ARCH_FOLDER)/$(ARCH) -type f -name '*.s')

CFILES += $(shell find ./$(KERNEL_DIR)/src -type f -name '*.c')
SFILES += $(shell find ./$(KERNEL_DIR)/src -type f -name '*.s')

OBJ    := $(SFILES:%.s=$(BUILD)/%.o)
OBJ    += $(CFILES:%.c=$(BUILD)/%.o)

QEMU = qemu-system-x86_64
QEMU_PARAMS_NODEBUG = -no-reboot -vga std -D ./log.txt -d int,guest_errors,in_asm -m 512M -boot d -M q35  -serial mon:stdio -m 1G -smp $(CORE_NUM) -cdrom
QEMU_PARAMS_DEBUG = -S -gdb tcp::9000

$(shell mkdir -p $(addprefix $(BUILD)/,$(SRCDIRS)))

all: ../iso_root

.PHONY: all clean NODEBUG DEBUG clean_deps ramdisk

NODEBUG: $(KERNEL).iso
	@$(QEMU) $(QEMU_PARAMS_NODEBUG) $<

DEBUG: $(KERNEL).iso
	@$(QEMU) $(QEMU_PARAMS_DEBUG) $(QEMU_PARAMS_NODEBUG) $<

../iso_root: $(KERNEL).elf limine/Makefile
	@mkdir -p $@
	@cp -r $(ISO_FILES) $@
	@echo 'Please initialize your system in the iso_root at the root of the project.'
	@echo 'Then run make iso at the root'

$(KERNEL).elf: $(OBJ)
	@ld $(INTERNALLDFLAGS) $(OBJ) -o $@

limine/Makefile:
	@git clone https://github.com/limine-bootloader/limine.git --branch=v2.0-branch-binary --depth=1
	@make -C limine

$(BUILD)/%.o: %.c
	@$(CC) -o $@ $(CFLAGS) $(INTERNALCFLAGS) -c $<

$(BUILD)/%.o: %.s
	@nasm $(ASMPARAM) -o $@ $<

clean:
	@rm -rf $(KERNEL).elf $(OBJ) image.hdd $(KERNEL).iso $(BUILD) log.txt iso_root

clean_deps: clean
	@rm -rf limine
