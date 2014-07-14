CPPFILES := $(shell find $(pwd) -type f -name "*.cpp")
ASMFILES := $(shell find $(pwd) -type f -name "*.asm")
CPPOBJFILES := $(patsubst %.cpp,%.o,$(CPPFILES))
ASMOBJFILES := $(patsubst %.asm,%.o,$(ASMFILES))
CPPFLAGS := -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
LDFLAGS :=

all: ../isofiles/boot/kernel.bin

debug: CPPFLAGS += -g
debug: LDFLAGS += -Map ../kernel.map
debug: ../isofiles/boot/kernel.bin

../isofiles/boot/kernel.bin: $(CPPOBJFILES) $(ASMOBJFILES)
	@echo "Linking..."
	@i686-elf-ld -T linker.ld -o ../isofiles/boot/kernel.bin $(CPPOBJFILES) $(ASMOBJFILES) $(LDFLAGS)

clean:
	-rm $(CPPOBJFILES) $(ASMOBJFILES)
	-rm ../isofiles/boot/kernel.bin
	-rm ../kernel.map

%.o: %.cpp
	@echo "g++  " $<
	@i686-elf-g++ -c $< -o $@ $(CPPFLAGS)

%.o: %.asm
	@echo "nasm " $<
	@nasm -felf $< -o $@
