#include "DescriptorTables.h"

GDTEntry DescriptorTables::_gdtEntries[5];
GDTPtr DescriptorTables::_gdtPtr;
IDTEntry DescriptorTables::_idtEntries[256];
IDTPtr DescriptorTables::_idtPtr;

void DescriptorTables::Init()
{
	InitGDT();
	InitIDT();
}

void DescriptorTables::InitGDT()
{
	_gdtPtr.Limit = (sizeof(GDTEntry) * 5) - 1;
	_gdtPtr.Base = (uint32_t)&_gdtEntries;
	
	GDTSetGate(0, 0, 0, 0, 0);
	GDTSetGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	GDTSetGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
	GDTSetGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
	GDTSetGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
	
	GDTFlush((uint32_t)&_gdtPtr);
}

void DescriptorTables::InitIDT()
{
	_idtPtr.Limit = (sizeof(IDTEntry) * 256) - 1;
	_idtPtr.Base = (uint32_t)&_idtEntries;
	
	memset(&_idtEntries, 0, sizeof(IDTEntry) * 256);
	
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);
	
	IDTSetGate(0, (uint32_t)isr0, 0x08, 0x8E);
	IDTSetGate(1, (uint32_t)isr1, 0x08, 0x8E);
	IDTSetGate(2, (uint32_t)isr2, 0x08, 0x8E);
	IDTSetGate(3, (uint32_t)isr3, 0x08, 0x8E);
	IDTSetGate(4, (uint32_t)isr4, 0x08, 0x8E);
	IDTSetGate(5, (uint32_t)isr5, 0x08, 0x8E);
	IDTSetGate(6, (uint32_t)isr6, 0x08, 0x8E);
	IDTSetGate(7, (uint32_t)isr7, 0x08, 0x8E);
	IDTSetGate(8, (uint32_t)isr8, 0x08, 0x8E);
	IDTSetGate(9, (uint32_t)isr9, 0x08, 0x8E);
	IDTSetGate(10, (uint32_t)isr10, 0x08, 0x8E);
	IDTSetGate(11, (uint32_t)isr11, 0x08, 0x8E);
	IDTSetGate(12, (uint32_t)isr12, 0x08, 0x8E);
	IDTSetGate(13, (uint32_t)isr13, 0x08, 0x8E);
	IDTSetGate(14, (uint32_t)isr14, 0x08, 0x8E);
	IDTSetGate(15, (uint32_t)isr15, 0x08, 0x8E);
	IDTSetGate(16, (uint32_t)isr16, 0x08, 0x8E);
	IDTSetGate(17, (uint32_t)isr17, 0x08, 0x8E);
	IDTSetGate(18, (uint32_t)isr18, 0x08, 0x8E);
	IDTSetGate(19, (uint32_t)isr19, 0x08, 0x8E);
	IDTSetGate(20, (uint32_t)isr20, 0x08, 0x8E);
	IDTSetGate(21, (uint32_t)isr21, 0x08, 0x8E);
	IDTSetGate(22, (uint32_t)isr22, 0x08, 0x8E);
	IDTSetGate(23, (uint32_t)isr23, 0x08, 0x8E);
	IDTSetGate(24, (uint32_t)isr24, 0x08, 0x8E);
	IDTSetGate(25, (uint32_t)isr25, 0x08, 0x8E);
	IDTSetGate(26, (uint32_t)isr26, 0x08, 0x8E);
	IDTSetGate(27, (uint32_t)isr27, 0x08, 0x8E);
	IDTSetGate(28, (uint32_t)isr28, 0x08, 0x8E);
	IDTSetGate(29, (uint32_t)isr29, 0x08, 0x8E);
	IDTSetGate(30, (uint32_t)isr30, 0x08, 0x8E);
	IDTSetGate(31, (uint32_t)isr31, 0x08, 0x8E);
	
	IDTSetGate(32, (uint32_t)irq0, 0x08, 0x8E);
	IDTSetGate(33, (uint32_t)irq1, 0x08, 0x8E);
	IDTSetGate(34, (uint32_t)irq2, 0x08, 0x8E);
	IDTSetGate(35, (uint32_t)irq3, 0x08, 0x8E);
	IDTSetGate(36, (uint32_t)irq4, 0x08, 0x8E);
	IDTSetGate(37, (uint32_t)irq5, 0x08, 0x8E);
	IDTSetGate(38, (uint32_t)irq6, 0x08, 0x8E);
	IDTSetGate(39, (uint32_t)irq7, 0x08, 0x8E);
	IDTSetGate(40, (uint32_t)irq8, 0x08, 0x8E);
	IDTSetGate(41, (uint32_t)irq9, 0x08, 0x8E);
	IDTSetGate(42, (uint32_t)irq10, 0x08, 0x8E);
	IDTSetGate(43, (uint32_t)irq11, 0x08, 0x8E);
	IDTSetGate(44, (uint32_t)irq12, 0x08, 0x8E);
	IDTSetGate(45, (uint32_t)irq13, 0x08, 0x8E);
	IDTSetGate(46, (uint32_t)irq14, 0x08, 0x8E);
	IDTSetGate(47, (uint32_t)irq15, 0x08, 0x8E);
	
	IDTSetGate(128, (uint32_t)irq128, 0x08, 0x8E);
	
	IDTFlush((uint32_t)&_idtPtr);
}

void DescriptorTables::GDTSetGate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
{
	_gdtEntries[num].BaseLow = (base & 0xFFFF);
	_gdtEntries[num].BaseMiddle = (base >> 16) & 0xFF;
	_gdtEntries[num].BaseHigh = (base >> 24) & 0xFF;
	
	_gdtEntries[num].LimitLow = (limit & 0xFFFF);
	_gdtEntries[num].Granularity = (limit >> 16) & 0x0F;
	
	_gdtEntries[num].Granularity |= granularity & 0xF0;
	_gdtEntries[num].Access = access;
}

void DescriptorTables::IDTSetGate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags)
{
	_idtEntries[num].BaseLow = base & 0xFFFF;
	_idtEntries[num].BaseHigh = (base >> 16) & 0xFFFF;
	
	_idtEntries[num].Selector = selector;
	_idtEntries[num].Always0 = 0;
	
	_idtEntries[num].Flags = flags;
}
