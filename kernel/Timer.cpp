#include "Timer.h"
#include "Terminal.h"

void Timer::Init(uint32_t frequency)
{
	uint32_t divisor = 1193180 / frequency;
	
	outb(0x43, 0x36);
	
	uint8_t low = (uint8_t)(divisor & 0xFF);
	uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
	
	outb(0x40, low);
	outb(0x40, high);
	
	Interrupts::RegisterInterruptHandler(IRQ0, &InterruptCallback);
}

void Timer::InterruptCallback(Registers* regs)
{
}
