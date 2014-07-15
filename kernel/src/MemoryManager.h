#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "Common.h"

class MemoryManager
{
public:
	static void Init();
	static uint32_t GetEndAddress();
	static void* Allocate(size_t);
	static void* AllocateAligned(size_t);
	static void* AllocatePhysical(size_t, uint32_t*);
	static void* AllocateAlignedPhysical(size_t, uint32_t*);
private:
	static void* Allocate(size_t, bool, uint32_t*);
	static uint32_t _endOfMemory;
};

#endif
