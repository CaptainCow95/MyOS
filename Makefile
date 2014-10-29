# all the modules that are part of the OS
MODULES := kernel initrd
# Kernel CPP Compiler
KernelCPPCompiler := ./compiler/bin/i686-elf-g++
# Kernal Linker
KernalLinker := ./compiler/bin/i686-elf-ld
# global cpp flags
CPPFLAGS_global := -MMD -I includes
# global asm flags
ASMFLAGS_global := -f elf
# global ld flags
LDFLAGS_global :=

# build the release iso
all: boot.iso

# build the debug iso
debug: debug-boot.iso

# build and run the release iso
run: boot.iso
	@toolchain-build/bochs-2.6.6/bochs -f bochsconfig -q

# build and run the debug iso
run-debug: debug-boot.iso
	@toolchain-build/bochs-2.6.6/bochs -f bochsconfig -q

# build the release iso
boot.iso: bin/release/kernel.bin bin/release/initrd
	cd ./bin/release && ./initrd ../../test.txt test.txt
	cp bin/release/kernel.bin isofiles/boot/kernel.bin
	cp bin/release/initrd.img isofiles/boot/initrd.img
	@echo "Generating ISO..."
	@genisoimage -R -quiet -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o boot.iso isofiles

# build the debug iso
debug-boot.iso: bin/debug/kernel.bin bin/debug/initrd
	cd ./bin/debug && ./initrd ../../test.txt test.txt
	cp bin/debug/kernel.bin isofiles/boot/kernel.bin
	cp bin/debug/initrd.img isofiles/boot/initrd.img
	@echo "Generating ISO..."
	@genisoimage -R -quiet -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o boot.iso isofiles

# template to include all the module build files
define MK_template
include $(1)/build.mk
endef

# template to include all the kernel module build rules
define RULES_KERNEL_template
$(1)/obj/debug/%.o: $(1)/src/%.cpp
	@mkdir -p ./$(1)/obj/debug
	@echo "g++  " $$<
	@$$(KernelCPPCompiler) $$(CPPFLAGS_global) $$(CPPFLAGS_$(1)) $$(CPPFLAGS_DEBUG_$(1)) -c $$< -o $$@

$(1)/obj/release/%.o: $(1)/src/%.cpp
	@mkdir -p ./$(1)/obj/release
	@echo "g++  " $$<
	@$$(KernelCPPCompiler) $$(CPPFLAGS_global) $$(CPPFLAGS_$(1)) $$(CPPFLAGS_RELEASE_$(1)) -c $$< -o $$@

$(1)/obj/debug/%.o: $(1)/src/%.asm
	@mkdir -p ./$(1)/obj/debug
	@echo "nasm " $$<
	@nasm $$(ASMFLAGS_global) $(ASMFLAGS_$(1)) $$(ASMFLAGS_DEBUG_$(1)) $$< -o $$@

$(1)/obj/release/%.o: $(1)/src/%.asm
	@mkdir -p ./$(1)/obj/release
	@echo "nasm " $$<
	@nasm $$(ASMFLAGS_global) $(ASMFLAGS_$(1)) $$(ASMFLAGS_RELEASE_$(1)) $$< -o $$@
endef

# template to include all helper module build rules
define RULES_HELPER_template
$(1)/obj/debug/%.o: $(1)/src/%.cpp
	@mkdir -p ./$(1)/obj/debug
	@echo "g++  " $$<
	@g++ $$(CPPFLAGS_global) $$(CPPFLAGS_$(1)) $$(CPPFLAGS_DEBUG_$(1)) -c $$< -o $$@

$(1)/obj/release/%.o: $(1)/src/%.cpp
	@mkdir -p ./$(1)/obj/release
	@echo "g++  " $$<
	@g++ $$(CPPFLAGS_global) $$(CPPFLAGS_$(1)) $$(CPPFLAGS_RELEASE_$(1)) -c $$< -o $$@
endef

# template to include all the program build steps
define PROGRAM_template
DEPENDENCIES := $(DEPENDENCIES) $(patsubst %,$(2)/obj/%.d,$(basename $($(2)_SOURCES)))

bin/debug/$(1): $(patsubst %,$(2)/obj/debug/%.o,$(basename $($(2)_SOURCES))) $(foreach library,$($(2)_LIBRARIES),lib/$(library))
	@mkdir -p ./bin/debug
	@$$(KernalLinker) $$(LDFLAGS_global) $$(LDFLAGS_$(2)) $$(LDFLAGS_DEBUG_$(2)) $$^ -o $$@

bin/release/$(1): $(patsubst %,$(2)/obj/release/%.o,$(basename $($(2)_SOURCES))) $(foreach library,$($(2)_LIBRARIES),lib/$(library))
	@mkdir -p ./bin/release
	@$$(KernalLinker) $$(LDFLAGS_global) $$(LDFLAGS_$(2)) $$(LDFLAGS_RELEASE_$(2)) $$^ -o $$@
endef

# template to include all the helper program build steps
define HELPER_PROGRAM_template
DEPENDENCIES := $(DEPENDENCIES) $(patsubst %,$(2)/obj/%.d,$(basename $($(2)_SOURCES)))

bin/debug/$(1): $(patsubst %,$(2)/obj/debug/%.o,$(basename $($(2)_SOURCES))) $(foreach library,$($(2)_LIBRARIES),lib/$(library))
	@mkdir -p ./bin/debug
	@g++ $$^ -o $$@

bin/release/$(1): $(patsubst %,$(2)/obj/release/%.o,$(basename $($(2)_SOURCES))) $(foreach library,$($(2)_LIBRARIES),lib/$(library))
	@mkdir -p ./bin/release
	@g++ $$^ -o $$@
endef

# template to add global links to the include files.
define INCLUDE_template
$(shell mkdir -p ./includes)
$(shell ln -s -f -n ../$(1)/include includes/$(1))
endef

# add the templates for each module
$(foreach module,$(MODULES),$(eval include $(module)/build.mk))
$(foreach module,$(MODULES),$(eval $(foreach binary,$($(module)_PROGRAM),$(call RULES_KERNEL_template,$(module)))))
$(foreach module,$(MODULES),$(eval $(foreach binary,$($(module)_HELPER_PROGRAM),$(call RULES_HELPER_template,$(module)))))
$(foreach module,$(MODULES),$(eval $(foreach binary,$($(module)_PROGRAM),$(call PROGRAM_template,$(binary),$(module)))))
$(foreach module,$(MODULES),$(eval $(foreach binary,$($(module)_HELPER_PROGRAM),$(call HELPER_PROGRAM_template,$(binary),$(module)))))
$(foreach module,$(MODULES),$(eval $(call INCLUDE_template,$(module))))

# include all dependencies
-include $(sort $(DEPENDENCIES))

# cleans up all .o files
clean:
	-rm $(foreach mod,$(MODULES),$(mod)/obj/release/*.o)
	-rm $(foreach mod,$(MODULES),$(mod)/obj/debug/*.o)

# cleans up all files created by the build process
clean-all:
	-rm boot.iso
	-rm isofiles/boot/kernel.bin
	-rm -r $(foreach mod,$(MODULES),$(mod)/obj) includes bin lib
