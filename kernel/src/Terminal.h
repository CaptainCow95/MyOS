#ifndef TERMINAL_H
#define TERMINAL_H

#include "Common.h"

/* Hardware text mode color constants. */
enum TerminalColor
{
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GRAY = 7,
	COLOR_DARK_GRAY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};

class Terminal
{
public:
	static void Init();
	static void SetColor(enum TerminalColor, enum TerminalColor);
	static void PutChar(char);
	static void Write(const char*);
	static void WriteDec(uint32_t);
	static void WriteDec(int32_t);
	static void WriteHex(uint32_t);
	static void WriteHex(int32_t);
private:
	static void PutChar(char, uint32_t, uint32_t);
	
	static uint32_t _row;
	static uint32_t _column;
	static uint8_t _color;
	static uint16_t* _buffer;
};

#endif
