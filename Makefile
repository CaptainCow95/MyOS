all: boot.iso

clean:
	-rm boot.iso
	cd kernel && $(MAKE) clean

run: boot.iso
	qemu-system-i386 -cdrom boot.iso

boot.iso:
	cd kernel && $(MAKE)
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o boot.iso isofiles
