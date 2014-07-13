#include "Interrupts.h"
#include "Terminal.h"

extern "C" void ISRHandler(Registers* regs)
{
	terminal_writestring("recieved interrupt!");
}
