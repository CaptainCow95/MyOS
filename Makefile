all: boot.iso

clean:
	-rm boot.iso
	cd kernel && $(MAKE) clean

run: boot.iso
	@bochs -f bochsconfig

boot.iso:
	@cd kernel && $(MAKE)
	@echo "Generating ISO..."
	@genisoimage -R -quiet -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o boot.iso isofiles
