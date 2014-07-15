#include "MemoryManager.h"

extern uint32_t endkernel;

uint32_t MemoryManager::_endOfMemory;

void MemoryManager::Init()
{
	_endOfMemory = (uint32_t)&endkernel;
}

uint32_t MemoryManager::GetEndAddress()
{
	return _endOfMemory;
}

void* MemoryManager::Allocate(size_t size)
{
	return Allocate(size, false, 0);
}

void* MemoryManager::AllocateAligned(size_t size)
{
	return Allocate(size, true, 0);
}

void* MemoryManager::AllocatePhysical(size_t size, uint32_t* physical)
{
	return Allocate(size, false, physical);
}

void* MemoryManager::AllocateAlignedPhysical(size_t size, uint32_t* physical)
{
	return Allocate(size, true, physical);
}

void* MemoryManager::Allocate(size_t size, bool align, uint32_t* physical)
{
	if(align && (_endOfMemory & 0xFFFFF000))
	{
		_endOfMemory &= 0xFFFFF000;
		_endOfMemory += 0x1000;
	}
	
	if(physical)
	{
		*physical = _endOfMemory;
	}
	
	uint32_t temp = _endOfMemory;
	_endOfMemory += size;
	return (void*)temp;
}
