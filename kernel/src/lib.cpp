#include "kernel/lib.h"

void printf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	asm volatile("movl %0, %%eax\n": :"i"(SYSTEMCALL_PRINTF));
	asm volatile("movl %0, %%ebx\n": :"m"(format));
	asm volatile("movl %0, %%ecx\n": :"m"(args));
	asm volatile("int $0x80\n");
	va_end(args);
}
