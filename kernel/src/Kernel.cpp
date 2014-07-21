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
	
	mb = (multiboot_info*)((uint32_t)mb + 0xC0000000);
	
	Terminal::Init();
	
	if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		PANIC("Multiboot magic value was not correct!");
	}
	
	DescriptorTables::Init();
	Interrupts::Init();
	Timer::Init(120);
	
	MemoryManager::Init(mb);
	
	uint32_t initrdStart = *(uint32_t*)(mb->mods_addr + 0xC0000000);
	uint32_t initrdEnd = *(uint32_t*)(mb->mods_addr + 4 + 0xC0000000);
	
	for(uint32_t i = initrdStart & 0xFFFFF000; i < initrdEnd; i += 0x1000)
	{
		MemoryManager::AllocatePage(MemoryManager::GetPage(i, true, MemoryManager::GetCurrentDirectory()), true, true, true, i);
	}
	
	uint32_t initrdLocation = (uint32_t)MemoryManager::AllocateAligned(initrdEnd - initrdStart);
	memcpy((void*)initrdLocation, (void*)initrdStart, initrdEnd - initrdStart);
	
	for(uint32_t i = initrdStart & 0xFFFFF000; i < initrdEnd; i += 0x1000)
	{
		MemoryManager::FreePage(MemoryManager::GetPage(i, false, MemoryManager::GetCurrentDirectory()));
	}
	
	root = Initrd::Init(initrdLocation);
	
	TaskManager::Init();
	
	asm volatile("sti");
	
	Terminal::Write("Ready!\n");
	
	TaskManager::CreateThread(&ReadFileSystem);
}
