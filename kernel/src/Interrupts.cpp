#include "Interrupts.h"
#include "Terminal.h"

InterruptHandler _interruptHandlers[256];

extern "C" void InterruptHandlerFunction(Registers* regs)
{
	if(regs->InterruptNumber >= 32)
	{
		if(regs->InterruptNumber >= 40)
		{
			outb(0xA0, 0x20);
		}
		
		outb(0x20, 0x20);
	}
	
	if(_interruptHandlers[regs->InterruptNumber] != 0)
	{
		InterruptHandler handler = _interruptHandlers[regs->InterruptNumber];
		handler(regs);
	}
}

void Interrupts::Init()
{
	for(uint32_t i = 0; i < 256; ++i)
	{
		_interruptHandlers[i] = 0;
	}
}

void Interrupts::RegisterInterruptHandler(uint8_t i, InterruptHandler handler)
{
	_interruptHandlers[i] = handler;
}
