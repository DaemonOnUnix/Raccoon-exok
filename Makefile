ARCH=x86_64
TARGET=NODEBUG

KERNEL=Raccoon-exokernel

all:
	@$(MAKE) $(TARGET) -C src -s
	@$(MAKE) iso -s

iso:
	@xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		-eltorito-alt-boot -e limine-eltorito-efi.bin \
		-no-emul-boot iso_root -o $(KERNEL).iso

clean:
	@$(MAKE) clean -C src -s
	@rm -rf iso_root
	@rm -rf $(KERNEL).iso