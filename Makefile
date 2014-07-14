KERNELFLAGS := 

all: boot.iso

debug: KERNELFLAGS += debug
debug: boot.iso

clean:
	-rm boot.iso
	@cd kernel && $(MAKE) clean

run: boot.iso
	@toolchain-build/bochs-2.6.6/bochs -f bochsconfig -q

boot.iso:
	@cd kernel && $(MAKE) $(KERNELFLAGS)
	@echo "Generating ISO..."
	@genisoimage -R -quiet -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o boot.iso isofiles
