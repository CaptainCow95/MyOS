CPPFLAGS_kernel := -ffreestanding -Wall -Wextra -fno-exceptions -fno-rtti
CPPFLAGS_DEBUG_kernel := -g
CPPFLAGS_RELEASE_kernel := -O2 -Werror
LDFLAGS_kernel := -T kernel/linker.ld
LDFLAGS_DEBUG_kernel := -Map bin/debug/kernel.map
LDFLAGS_RELEASE_kernel :=

kernel_PROGRAM := kernel.bin

kernel_LIBRARIES :=

kernel_SOURCES := $(patsubst kernel/src/%,%,$(wildcard kernel/src/*.cpp) $(wildcard kernel/src/*.asm))
