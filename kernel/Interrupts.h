#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "Common.h"

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

struct Registers
{
	uint32_t ds;
	uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax;
	uint32_t InterruptNumber, ErrorCode;
	uint32_t eip, cs, eflags, esp, ss;
};

typedef void (*InterruptHandler)(Registers*);

class Interrupts
{
public:
	static void RegisterInterruptHandler(uint8_t n, InterruptHandler handler);
	static void Init();
};

#endif
