#ifndef SYSTEMCALLS_H
#define SYSTEMCALLS_H

#include "Common.h"
#include "Interrupts.h"
#include "kernel/lib.h"

class SystemCalls
{
public:
	static void Init();
private:
	static void InterruptCallback(Registers* regs);
	static void SystemCallPrintf(const char*, va_list);
};

#endif
