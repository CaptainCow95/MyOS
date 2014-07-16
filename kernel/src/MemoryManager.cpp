#include "MemoryManager.h"
#include "Terminal.h"

extern uint32_t endkernel;

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

// Size of the initial heap and the size to expand
#define HEAP_STEP 0x100000
// Some reserved space for the new page tables as needed
#define HEAP_RESERVE 0x2000

uint32_t MemoryManager::_endOfMemory;
MemoryHeader* MemoryManager::_firstHeader;
MemoryHeader* MemoryManager::_lastHeader;
uint32_t* MemoryManager::_frames;
uint32_t MemoryManager::_nframes;
PageDirectory* MemoryManager::_kernelDirectory;
PageDirectory* MemoryManager::_currentDirectory;

void MemoryManager::Init(multiboot_info* mb)
{
	_endOfMemory = (uint32_t)&endkernel;
	
	// align the memory
	_endOfMemory &= 0xFFFFF000;
	_endOfMemory += 0x1000;
	_firstHeader = (MemoryHeader*)_endOfMemory;
	_firstHeader->Used = false;
	_firstHeader->Size = HEAP_STEP - sizeof(MemoryHeader);
	_firstHeader->Previous = 0;
	_firstHeader->Next = 0;
	_firstHeader->PreviousFree = 0;
	_firstHeader->NextFree = 0;
	_lastHeader = _firstHeader;
	_endOfMemory += HEAP_STEP + HEAP_RESERVE;
	
	multiboot_mmap_entry* mmapEntry = (multiboot_mmap_entry*)mb->mmap_addr;
	uint32_t memoryEndPage = 0;
	while((uint32_t)mmapEntry < (uint32_t)mb->mmap_addr + (uint32_t)mb->mmap_length)
	{
		if(mmapEntry->type == MULTIBOOT_MEMORY_AVAILABLE && memoryEndPage < (uint32_t)mmapEntry->addr + (uint32_t)mmapEntry->len)
		{
			memoryEndPage = (uint32_t)mmapEntry->addr + (uint32_t)mmapEntry->len;
		}
		
		mmapEntry = (multiboot_mmap_entry*)((uint32_t)mmapEntry + (uint32_t)mmapEntry->size + sizeof(mmapEntry->size));
	}
	
	_nframes = memoryEndPage / 0x1000;
	_frames = (uint32_t*)Allocate(INDEX_FROM_BIT(_nframes));
	memset(_frames, 0, INDEX_FROM_BIT(_nframes));
	
	_kernelDirectory = (PageDirectory*)AllocateAligned(sizeof(PageDirectory));
	memset(_kernelDirectory, 0, sizeof(PageDirectory));
	_currentDirectory = _kernelDirectory;
	
	// Identity map memory
	uint32_t i = 0;
	while(i < _endOfMemory)
	{
		AllocatePage(GetPage(i, true, _kernelDirectory), true, true);
		i += 0x1000;
	}
	
	// Now that everything has been identity mapped,
	// mark the rest of unusable/reserved memory
	mmapEntry = (multiboot_mmap_entry*)mb->mmap_addr;
	while((uint32_t)mmapEntry < (uint32_t)mb->mmap_addr + (uint32_t)mb->mmap_length)
	{
		if(mmapEntry->type != MULTIBOOT_MEMORY_AVAILABLE && (uint32_t)mmapEntry->addr < memoryEndPage)
		{
			for(uint32_t j = (uint32_t)mmapEntry->addr; j < (uint32_t)mmapEntry->addr + (uint32_t)mmapEntry->len; j += 0x1000)
			{
				SetFrame(j);
			}
			
			SetFrame((uint32_t)mmapEntry->addr + (uint32_t)mmapEntry->len);
		}
		
		mmapEntry = (multiboot_mmap_entry*)((uint32_t)mmapEntry + (uint32_t)mmapEntry->size + sizeof(mmapEntry->size));
	}
	
	Interrupts::RegisterInterruptHandler(14, &PageFault);
	
	SwitchPageDirectory(_kernelDirectory);
	
	// enable paging
	uint32_t cr0;
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "r"(cr0));
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
	MemoryHeader* header = _firstHeader;
	if(header->Used)
	{
		header = header->NextFree;
	}
	
	while(header != 0)
	{
		if(header->Size < size)
		{
			header = header->NextFree;
			continue;
		}
		
		uint32_t newHeaderLocation;
		uint32_t returnAddress;
		
		// If this is to be a page table
		if(align)
		{
			// Is this suitable for a page directory
			uint32_t address = (uint32_t)header + sizeof(MemoryHeader);
			
			if(address & 0xFFFFF000)
			{
				address &= 0xFFFFF000;
				address += 0x1000;
			}
			
			if(header->Size < ((address - (uint32_t)header) + size))
			{
				header = header->NextFree;
				continue;
			}
			
			returnAddress = address;
			newHeaderLocation = address + size;
			
			if(((uint32_t)header + sizeof(MemoryHeader)) != returnAddress)
			{
				// move header so we can find it if we free the memory
				MemoryHeader* movedHeader = (MemoryHeader*)(returnAddress - sizeof(MemoryHeader));
				movedHeader->NextFree = header->NextFree;
				movedHeader->PreviousFree = header->PreviousFree;
				movedHeader->Next = header->Next;
				movedHeader->Previous = header->Previous;
				movedHeader->Size = header->Size;
				movedHeader->Used = header->Used;
				movedHeader->Size -= (uint32_t)movedHeader - (uint32_t)header;
				if(movedHeader->Previous != 0)
				{
					movedHeader->Previous->Next = movedHeader;
					movedHeader->Previous->Size += (uint32_t)movedHeader - (uint32_t)header;
				}
				
				if(movedHeader->Next != 0)
				{
					movedHeader->Next->Previous = movedHeader;
				}
				
				if(movedHeader->PreviousFree != 0)
				{
					movedHeader->PreviousFree->NextFree = movedHeader;
				}
				
				if(movedHeader->NextFree != 0)
				{
					movedHeader->NextFree->PreviousFree = movedHeader;
				}
				
				if(header == _firstHeader)
				{
					_firstHeader = movedHeader;
				}
				
				header = movedHeader;
			}
		}
		else
		{
			newHeaderLocation = (uint32_t)header + sizeof(MemoryHeader) + size;
			returnAddress = (uint32_t)header + sizeof(MemoryHeader);
		}
		
		// We can use this memory location
		header->Used = true;
		if(header->NextFree != 0)
		{
			header->NextFree->PreviousFree = header->PreviousFree;
		}
		
		if(header->PreviousFree != 0)
		{
			header->PreviousFree->NextFree = header->NextFree;
		}
		
		// Can we fit a new header?
		if(newHeaderLocation & 0xFFFFFFFB)
		{
			newHeaderLocation &= 0xFFFFFFFB;
			newHeaderLocation += 0x4;
		}
		
		uint32_t newSize = sizeof(MemoryHeader) + (newHeaderLocation - (uint32_t)header);
		if(header->Size > newSize)
		{
			MemoryHeader* newHeader = (MemoryHeader*)newHeaderLocation;
			newHeader->Used = false;
			newHeader->Size = header->Size - (newHeaderLocation - (uint32_t)header);
			header->Size = newHeaderLocation - (uint32_t)header - sizeof(MemoryHeader);
			newHeader->Previous = header;
			newHeader->Next = header->Next;
			header->Next = newHeader;
			if(newHeader->Next != 0)
			{
				newHeader->Next->Previous = newHeader;
			}
			
			InsertIntoFreeList(newHeader);
			
			if(header == _lastHeader)
			{
				_lastHeader = newHeader;
			}
		}
		
		if(physical)
		{
			*physical = returnAddress;
		}
		
		return (void*)returnAddress;
	}
	
	if(ExpandHeap())
	{
		return Allocate(size, align, physical);
	}
	else
	{
		PANIC("OUT OF MEMORY!!!");
		return 0;
	}
}

void MemoryManager::Free(void* p)
{
	if(p == 0)
	{
		return;
	}
	
	MemoryHeader* header = (MemoryHeader*)((uint32_t)p - sizeof(MemoryHeader));
	header->Used = false;
	Combine(header);
	
	if(_lastHeader->Size > HEAP_STEP && _firstHeader != _lastHeader)
	{
		ContractHeap();
	}
}

void MemoryManager::InsertIntoFreeList(MemoryHeader* newHeader)
{
	newHeader->NextFree = 0;
	newHeader->PreviousFree = 0;

	MemoryHeader* header = _firstHeader;
	if(header->Used)
	{
		if(header->NextFree == 0)
		{
			header->NextFree = newHeader;
			newHeader->PreviousFree = header;
			return;
		}
		else if(header->NextFree->Size > newHeader->Size)
		{
			newHeader->PreviousFree = header;
			newHeader->NextFree = header->NextFree;
			header->NextFree->PreviousFree = newHeader;
			header->NextFree = newHeader;
			return;
		}
		
		header = header->NextFree;
	}
	
	while(header->NextFree != 0)
	{
		if(header->Size < newHeader->Size && header->NextFree->Size >= newHeader->Size)
		{
			newHeader->PreviousFree = header;
			newHeader->NextFree = header->NextFree;
			header->NextFree->PreviousFree = newHeader;
			header->NextFree = newHeader;
			return;
		}
		
		header = header->NextFree;
	}
	
	header->NextFree = newHeader;
	newHeader->PreviousFree = header;
	newHeader->NextFree = 0;
}

void MemoryManager::Combine(MemoryHeader* header)
{
	if(header->Previous != 0 && !header->Previous->Used)
	{
		// combine left
		MemoryHeader* previous = header->Previous;
		
		if(previous->PreviousFree != 0)
		{
			previous->PreviousFree->NextFree = previous->NextFree;
		}
		
		if(previous->NextFree != 0)
		{
			previous->NextFree->PreviousFree = previous->PreviousFree;
		}
		
		previous->Next = header->Next;
		previous->Size += header->Size + sizeof(MemoryHeader);
		
		if(previous->Next != 0)
		{
			previous->Next->Previous = previous;
		}
		
		header = previous;
	}
	
	if(header->Next != 0 && !header->Next->Used)
	{
		// combine right
		MemoryHeader* next = header->Next;
		
		if(next->PreviousFree != 0)
		{
			next->PreviousFree->NextFree = next->NextFree;
		}
		
		if(next->NextFree != 0)
		{
			next->NextFree->PreviousFree = next->PreviousFree;
		}
		
		header->Next = next->Next;
		header->Size += next->Size + sizeof(MemoryHeader);
		
		if(header->Next != 0)
		{
			header->Next->Previous = header;
		}
	}
	
	InsertIntoFreeList(header);
}

bool MemoryManager::ExpandHeap()
{
	uint32_t endOfHeap = (uint32_t)_lastHeader + sizeof(MemoryHeader) + _lastHeader->Size;
	if(endOfHeap > 0xFFFFFFFF - HEAP_STEP)
	{
		return false;
	}
	
	// 1. create free header in the reserve space
	MemoryHeader* header = (MemoryHeader*)endOfHeap;
	header->Used = false;
	header->Size = HEAP_RESERVE - sizeof(MemoryHeader);
	header->Previous = _lastHeader;
	header->Next = 0;
	_lastHeader = header;
	Combine(header);
	
	// 3. allocate all new pages for the heap
	for(uint32_t i = endOfHeap; i < endOfHeap + HEAP_STEP + HEAP_RESERVE; i += 0x1000)
	{
		AllocatePage(GetPage(i, true, _currentDirectory), true, true);
	}
	
	// 4. add new free header with the remaining space
	header = (MemoryHeader*)endOfHeap + HEAP_RESERVE;
	header->Used = false;
	header->Size = HEAP_STEP - HEAP_RESERVE - sizeof(MemoryHeader);
	header->Previous = _lastHeader;
	header->Next = 0;
	_lastHeader = header;
	Combine(header);
	
	return true;
}

void MemoryManager::ContractHeap()
{
	uint32_t endOfHeap = (uint32_t)_lastHeader + sizeof(MemoryHeader) + _lastHeader->Size + HEAP_RESERVE;
	_lastHeader->Size -= HEAP_STEP;
	for(uint32_t i = endOfHeap - HEAP_STEP; i < endOfHeap; i += 0x1000)
	{
		FreePage(GetPage(i, false, _currentDirectory));
	}
}

void MemoryManager::SetFrame(uint32_t frameAddress)
{
	uint32_t frame = frameAddress / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	_frames[idx] |= (0x1 << off);
}

void MemoryManager::ClearFrame(uint32_t frameAddress)
{
	uint32_t frame = frameAddress / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	_frames[idx] &= ~(0x1 << off);
}

uint32_t MemoryManager::TestFrame(uint32_t frameAddress)
{
	uint32_t frame = frameAddress / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	return (_frames[idx] & (0x1 << off));
}

uint32_t MemoryManager::FirstFrame()
{
	for(uint32_t i = 0; i < INDEX_FROM_BIT(_nframes); ++i)
	{
		if(_frames[i] != 0xFFFFFFFF)
		{
			for(uint32_t j = 0; j < 32; ++j)
			{
				uint32_t toTest = 0x1 << j;
				if(!(_frames[i] & toTest))
				{
					return i * 4 * 8 + j;
				}
			}
		}
	}
	
	return -1;
}

void MemoryManager::AllocatePage(Page* page, bool isKernel, bool isWriteable)
{
	if(page->Frame != 0)
	{
		return;
	}
	else
	{
		uint32_t idx = FirstFrame();
		if(idx == (uint32_t)-1)
		{
			PANIC("No free pages to allocate!");
		}
		
		SetFrame(idx * 0x1000);
		page->Present = 1;
		page->ReadWrite = isWriteable;
		page->User = isKernel;
		page->Frame = idx;
	}
}

void MemoryManager::FreePage(Page* page)
{
	uint32_t frame;
	if(!(frame = page->Frame))
	{
		// there was no allocated frame.
		return;
	}
	else
	{
		ClearFrame(frame);
		page->Frame = 0x0;
	}
}

void MemoryManager::SwitchPageDirectory(PageDirectory* directory)
{
	_currentDirectory = directory;
	asm volatile("mov %0, %%cr3":: "r"(directory->TablesPhysical));
}

Page* MemoryManager::GetPage(uint32_t address, bool make, PageDirectory* directory)
{
	address /= 0x1000;
	uint32_t tableIdx = address / 1024;
	if(directory->Tables[tableIdx])
	{
		return &directory->Tables[tableIdx]->Pages[address % 1024];
	}
	else if(make)
	{
		uint32_t temp;
		directory->Tables[tableIdx] = (PageTable*)AllocateAlignedPhysical(sizeof(PageTable), &temp);
		memset(directory->Tables[tableIdx], 0, 0x1000);
		directory->TablesPhysical[tableIdx] = temp | 0x7;
		return &directory->Tables[tableIdx]->Pages[address % 1024];
	}
	else
	{
		return 0;
	}
}

void MemoryManager::PageFault(Registers* regs)
{
	uint32_t faultingAddress;
	asm("mov %%cr2, %0" : "=r"(faultingAddress));
	
	bool present = !(regs->ErrorCode & 0x1);
	bool readWrite = regs->ErrorCode & 0x2;
	bool user = regs->ErrorCode & 0x4;
	bool reserved = regs->ErrorCode & 0x8;
	
	Terminal::Write("\n\nPage Fault! (");
	if(present) Terminal::Write("not present");
	if(readWrite) Terminal::Write("read-only");
	if(user) Terminal::Write("user-mode");
	if(reserved) Terminal::Write("reserved");
	Terminal::Write(") at ");
	Terminal::WriteHex(faultingAddress);
	Terminal::Write("\n");
	PANIC("Page fault!");
}
