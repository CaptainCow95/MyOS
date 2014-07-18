#include "DescriptorTables.h"
#include "Initrd.h"
#include "Interrupts.h"
#include "MemoryManager.h"
#include "Multiboot.h"
#include "TaskManager.h"
#include "Terminal.h"
#include "Timer.h"

extern uint32_t endkernel;

FileSystemNode* root;

void ReadFileSystem()
{
	uint32_t i = 0;
	FileSystemNode* node = 0;
	while((node = FileSystem::ReadDirectory(root, i)) != 0)
	{
		Terminal::Write("Found file ");
		Terminal::Write(node->Name);
		
		Terminal::Write("\n\t contents: \"");
		char buf[256];
		uint32_t size = FileSystem::Read(node, 0, 256, (uint8_t*)buf);
		for(uint32_t j = 0; j < size; ++j)
		{
			Terminal::PutChar(buf[j]);
		}
		
		Terminal::Write("\"\n");
		++i;
	}
}

extern "C" void kernel_main()
{
	extern uint32_t magic;
	extern multiboot_info* mb;
	
	Terminal::Init();
	
	if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		PANIC("Multiboot magic value was not correct!");
	}
	
	DescriptorTables::Init();
	Interrupts::Init();
	Timer::Init(120);
	
	uint32_t endOfMemory = (uint32_t)&endkernel;
	uint32_t initrdLocation = *((uint32_t*)mb->mods_addr);
	uint32_t initrdEnd = *(uint32_t*)(mb->mods_addr + 4);
	
	// Copy the initrd down near the kernel so we don't have
	// to worry about where the memory is and overriding it.
	// Also page align so there are absolutely no alignment issues.
	endOfMemory &= 0xFFFFF000;
	endOfMemory += 0x1000;
	memcpy((void*)endOfMemory, (void*)initrdLocation, initrdEnd - initrdLocation);
	initrdLocation = endOfMemory;
	endOfMemory += initrdEnd - initrdLocation;
	
	MemoryManager::Init(endOfMemory, mb);
	
	TaskManager::Init();
	
	root = Initrd::Init(initrdLocation);
	
	asm volatile("sti");
	
	Terminal::Write("Ready!\n");
	
	TaskManager::CreateThread(&ReadFileSystem);
}
