#ifndef LIB_H
#define LIB_H

#define SYSTEMCALL_PRINTF 0x0

typedef __builtin_va_list va_list;
#define va_start(LIST,ARG) __builtin_va_start(LIST,ARG)
#define va_end(LIST) __builtin_va_end(LIST)
#define va_arg(LIST,TYPE) __builtin_va_arg(LIST,TYPE)

void printf(const char* ...);

#endif
