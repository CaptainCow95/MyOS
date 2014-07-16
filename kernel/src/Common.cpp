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
	
	Terminal::Write("\n\nPanic(");
	Terminal::Write(message);
	Terminal::Write(") at ");
	Terminal::Write(file);
	Terminal::Write(":");
	Terminal::WriteDec(line);
	Terminal::Write(")");
	
	for(;;);
}

void itoa(int32_t value, char* str, int32_t base)
{
	static char num[] = "0123456789abcdef";
	char* wstr = str;
	
	if(base < 2 || base > 16)
	{
		*wstr = '\0';
		return;
	}
	
	bool negative = value < 0;
	
	if(value < 0)
	{
		value = -value;
	}
	
	do
	{
		*wstr++ = num[value % base];
	} while(value /= base);
	
	if(negative)
	{
		*wstr++ = '-';
	}
	
	*wstr = '\0';
	
	char* begin = str;
	char* end = wstr - 1;
	char aux;
	while(end > begin)
	{
		aux = *end;
		*end-- = *begin;
		*begin++ = aux;
	}
}

void itoa(uint32_t value, char* str, int32_t base)
{
	static char num[] = "0123456789abcdef";
	char* wstr = str;
	
	if(base < 2 || base > 16)
	{
		*wstr = '\0';
		return;
	}
	
	do
	{
		*wstr++ = num[value % base];
	} while(value /= base);
	
	*wstr = '\0';
	
	char* begin = str;
	char* end = wstr - 1;
	char aux;
	while(end > begin)
	{
		aux = *end;
		*end-- = *begin;
		*begin++ = aux;
	}
}
