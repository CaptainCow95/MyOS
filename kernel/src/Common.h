#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>

void outb(uint16_t port, uint8_t val);
void* memset(void* s, int32_t c, size_t n);
size_t strlen(const char* str);

#endif
