ARCH=x86_64
TARGET=

KERNEL=Raccoon-exokernel

QEMU = qemu-system-x86_64
CORE_NUM = 4

QEMU_BASE_PARAMS_NODEBUG = -no-reboot -D ./log.txt -d int,guest_errors,in_asm -m 512M -boot d -M q35  -serial mon:stdio -m 1G -smp $(CORE_NUM) -cdrom

QEMU_PARAMS_NOGRAPHICS = -nographic $(QEMU_BASE_PARAMS_NODEBUG)
QEMU_PARAMS_GRAPHICS = -vga std $(QEMU_BASE_PARAMS_NODEBUG)

all:
	@$(MAKE) $(TARGET) -C src -s

iso:
	@xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		-eltorito-alt-boot -e limine-eltorito-efi.bin \
		-no-emul-boot iso_root -o $(KERNEL).iso

test:
	@$(QEMU) $(QEMU_PARAMS_GRAPHICS) $(KERNEL).iso

test-nographics:
	@$(QEMU) $(QEMU_PARAMS_NOGRAPHICS) $(KERNEL).iso

clean:
	@$(MAKE) clean -C src -s
	@rm -rf iso_root
	@rm -rf $(KERNEL).iso
	@rm -f log.txt