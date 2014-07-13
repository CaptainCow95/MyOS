#include "Common.h"

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
