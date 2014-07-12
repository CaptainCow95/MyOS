CPPFILES := $(shell find $(pwd) -type f -name "*.cpp")
ASMFILES := $(shell find $(pwd) -type f -name "*.asm")
CPPOBJFILES := $(patsubst %.cpp,%.o,$(CPPFILES))
ASMOBJFILES := $(patsubst %.asm,%.o,$(ASMFILES))

all: ../isofiles/boot/kernel.bin

../isofiles/boot/kernel.bin: $(CPPOBJFILES) $(ASMOBJFILES)
	i686-elf-ld -T linker.ld -o ../isofiles/boot/kernel.bin $(CPPOBJFILES) $(ASMOBJFILES)

clean:
	-rm $(CPPOBJFILES) $(ASMOBJFILES)
	-rm ../isofiles/boot/kernel.bin

%.o: %.cpp
	i686-elf-g++ -c $< -o $@ -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti

%.o: %.asm
	nasm -felf $< -o $@
