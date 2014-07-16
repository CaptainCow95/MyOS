#include "DescriptorTables.h"
#include "Interrupts.h"
#include "MemoryManager.h"
#include "Multiboot.h"
#include "Terminal.h"
#include "Timer.h"
 
#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main()
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
	
	MemoryManager::Init(mb);
	
	Terminal::Write("Ready!\n");
}
