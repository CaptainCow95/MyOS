#include "DescriptorTables.h"
#include "Interrupts.h"
#include "Terminal.h"
#include "Timer.h"
 
#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main()
{
	terminal_initialize();
	terminal_writestring("Hello, kernel World!");
	DescriptorTables::Init();
	Interrupts::Init();
	Timer::Init(120);
}
