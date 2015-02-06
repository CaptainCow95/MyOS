#include "SystemCalls.h"
#include "Terminal.h"

void SystemCalls::Init()
{
	Interrupts::RegisterInterruptHandler(IRQ128, &InterruptCallback);
}

void SystemCalls::InterruptCallback(Registers* regs)
{
	switch(regs->eax)
	{
		case SYSTEMCALL_PRINTF:
			SystemCallPrintf((const char*)regs->ebx, (va_list)regs->ecx);
			break;
	}
}

void SystemCalls::SystemCallPrintf(const char* format, va_list args)
{
	int i = 0;
	
	while(format[i] != 0)
	{
		switch(format[i])
		{
			case '%':
				if(format[++i] == 0)
				{
					break;
				}
				
				switch(format[i])
				{
					case 'c':
						Terminal::PutChar((char)va_arg(args, int));
						break;
					case 's':
						Terminal::Write((char*)va_arg(args, int));
						break;
					case 'u':
						Terminal::WriteDec((uint32_t)va_arg(args, int));
						break;
					case 'd':
						Terminal::WriteDec((int32_t)va_arg(args, int));
						break;
					case 'x':
						Terminal::WriteHex((uint32_t)va_arg(args, int));
						break;
				}
				
				break;
			default:
				Terminal::PutChar(format[i]);
				break;
		}
		
		++i;
	}
}
