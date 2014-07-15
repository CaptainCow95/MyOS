#include "Common.h"
#include "Terminal.h"

void outb(uint16_t port, uint8_t val)
{
	asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

void* memset(void* s, int32_t c, size_t n)
{
	unsigned char* p = (unsigned char*)s;
	while(n--)
		*p++ = (unsigned char)c;
	return s;
}

size_t strlen(const char* str)
{
	size_t ret = 0;
	while ( str[ret] != 0 )
		ret++;
	return ret;
}

void panic(const char* message, const char* file, uint32_t line)
{
	asm volatile("cli");
	
	terminal_writestring("PANIC(");
	terminal_writestring(message);
	terminal_writestring(") at ");
	terminal_writestring(file);
	terminal_writestring(":");
	// write line
	
	for(;;);
}
