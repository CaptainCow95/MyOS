#ifndef DESCRIPTORTABLES_H
#define DESCRIPTORTABLES_H

#include "Common.h"

struct GDTEntry
{
	uint16_t LimitLow;
	uint16_t BaseLow;
	uint8_t BaseMiddle;
	uint8_t Access;
	uint8_t Granularity;
	uint8_t BaseHigh;
} __attribute__((packed));

struct GDTPtr
{
	uint16_t Limit;
	uint32_t Base;
} __attribute__((packed));

struct IDTEntry
{
	uint16_t BaseLow;
	uint16_t Selector;
	uint8_t Always0;
	uint8_t Flags;
	uint16_t BaseHigh;
} __attribute__((packed));

struct IDTPtr
{
	uint16_t Limit;
	uint32_t Base;
} __attribute__((packed));

extern "C" void GDTFlush(uint32_t);
extern "C" void IDTFlush(uint32_t);
extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();
extern "C" void irq0();
extern "C" void irq1();
extern "C" void irq2();
extern "C" void irq3();
extern "C" void irq4();
extern "C" void irq5();
extern "C" void irq6();
extern "C" void irq7();
extern "C" void irq8();
extern "C" void irq9();
extern "C" void irq10();
extern "C" void irq11();
extern "C" void irq12();
extern "C" void irq13();
extern "C" void irq14();
extern "C" void irq15();
extern "C" void irq128();

class DescriptorTables
{
public:
	static void Init();
private:
	static void InitGDT();
	static void InitIDT();
	static void GDTSetGate(int32_t, uint32_t, uint32_t, uint8_t, uint8_t);
	static void IDTSetGate(uint8_t, uint32_t, uint16_t, uint8_t);
	
	static GDTEntry _gdtEntries[5];
	static GDTPtr _gdtPtr;
	static IDTEntry _idtEntries[256];
	static IDTPtr _idtPtr;
};

#endif
