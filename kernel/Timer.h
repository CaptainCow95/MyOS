#ifndef TIMER_H
#define TIMER_H

#include "Common.h"
#include "Interrupts.h"

class Timer
{
public:
	static void Init(uint32_t frequency);
private:
	static void InterruptCallback(Registers* regs);
};

#endif
