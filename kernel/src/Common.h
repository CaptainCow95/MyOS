#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>

void outb(uint16_t, uint8_t);
void* memset(void*, int32_t, size_t);
void* memcpy(void*, const void*, size_t);
size_t strlen(const char*);
char* strcpy(char*, const char*);
void itoa(int32_t, char*, int32_t);
void itoa(uint32_t, char*, int32_t);

#define PANIC(msg) panic(msg, __FILE__, __LINE__);

void panic(const char*, const char*, uint32_t);

#endif
