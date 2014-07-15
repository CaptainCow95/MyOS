#include "DescriptorTables.h"
#include "Interrupts.h"
#include "MemoryManager.h"
#include "Paging.h"
#include "Terminal.h"
#include "Timer.h"
 
#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main()
{
	extern uint32_t magic;
	extern void* mbd;
	
	terminal_initialize();
	
	if(magic != 0x2BADB002)
	{
		PANIC("Magic value is not 0x2BADB002!");
	}
	
	terminal_writestring("Hello, kernel World!");
	DescriptorTables::Init();
	Interrupts::Init();
	Timer::Init(120);
	
	MemoryManager::Init();
	Paging::Init();
}
