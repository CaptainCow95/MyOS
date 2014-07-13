#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "Common.h"

struct Registers
{
	uint32_t ds;
	uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax;
	uint32_t InterruptNumber, ErrorCode;
	uint32_t eip, cs, eflags, esp, ss;
};

#endif
