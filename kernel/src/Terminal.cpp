#include "Terminal.h"

#define TERMINAL_HEIGHT 25
#define TERMINAL_WIDTH 80

uint32_t Terminal::_row;
uint32_t Terminal::_column;
uint8_t Terminal::_color;
uint16_t* Terminal::_buffer;

void Terminal::Init()
{
	_row = 0;
	_column = 0;
	SetColor(COLOR_LIGHT_GRAY, COLOR_BLACK);
	_buffer = (uint16_t*)0xC00B8000;
	for(uint32_t y = 0; y < TERMINAL_HEIGHT; ++y)
	{
		for(uint32_t x = 0; x < TERMINAL_WIDTH; ++x)
		{
			PutChar(' ', y, x);
		}
	}
}

void Terminal::SetColor(enum TerminalColor foreground, enum TerminalColor background)
{
	_color = foreground | background << 4;
}

void Terminal::PutChar(char c, uint32_t y, uint32_t x)
{
	_buffer[y * TERMINAL_WIDTH + x] = ((uint16_t)c) | (((uint16_t)_color) << 8);
}

void Terminal::PutChar(char c)
{
	if(c == '\n')
	{
		++_row;
		_column = 0;
	}
	else if(c == '\r')
	{
		_column = 0;
	}
	else if(c == '\t')
	{
		_column += 4 - _column % 4;
	}
	else
	{
		PutChar(c, _row, _column);
		++_column;
	}
	if(_column == TERMINAL_WIDTH)
	{
		_column = 0;
		++_row;
	}
	
	if(_row == TERMINAL_HEIGHT)
	{
		for(uint32_t y = 0; y < TERMINAL_HEIGHT - 1; ++y)
		{
			for(uint32_t x = 0; x < TERMINAL_WIDTH; ++x)
			{
				_buffer[y * TERMINAL_WIDTH + x] = _buffer[(y + 1) * TERMINAL_WIDTH + x];
			}
		}
		
		for(uint32_t x = 0; x < TERMINAL_WIDTH; ++x)
		{
			PutChar(' ', TERMINAL_HEIGHT - 1, x);
		}
		
		--_row;
	}
	
	uint16_t cursorPosition = _row * 80 + _column;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t)(cursorPosition & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t)((cursorPosition >> 8) & 0xFF));
}

void Terminal::Write(const char* data)
{
	uint32_t length = strlen(data);
	for(uint32_t i = 0; i < length; ++i)
	{
		PutChar(data[i]);
	}
}

void Terminal::WriteDec(int32_t num)
{
	char str[12];
	itoa(num, str, 10);
	Write(str);
}

void Terminal::WriteDec(uint32_t num)
{
	char str[11];
	itoa(num, str, 10);
	Write(str);
}

void Terminal::WriteHex(int32_t num)
{
	char str[10];
	itoa(num, str, 16);
	if(str[0] == '-')
	{
		PutChar('-');
		PutChar('0');
		PutChar('x');
		Write(&str[1]);
	}
	else
	{
		PutChar('0');
		PutChar('x');
		Write(str);
	}
}

void Terminal::WriteHex(uint32_t num)
{
	char str[9];
	itoa(num, str, 16);
	PutChar('0');
	PutChar('x');
	int32_t length = strlen(str);
	while(length < 8)
	{
		PutChar('0');
		++length;
	}
	Write(str);
}
