#include "MemoryManager.h"
#include "Paging.h"
#include "Terminal.h"

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

uint32_t* Paging::_frames;
uint32_t Paging::_nframes;
PageDirectory* Paging::_kernelDirectory;
PageDirectory* Paging::_currentDirectory;

void Paging::SetFrame(uint32_t frameAddress)
{
	uint32_t frame = frameAddress / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	_frames[idx] |= (0x1 << off);
}

void Paging::ClearFrame(uint32_t frameAddress)
{
	uint32_t frame = frameAddress / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	_frames[idx] &= ~(0x1 << off);
}

uint32_t Paging::TestFrame(uint32_t frameAddress)
{
	uint32_t frame = frameAddress / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	return (_frames[idx] & (0x1 << off));
}

uint32_t Paging::FirstFrame()
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

void Paging::AllocatePage(Page* page, bool isKernel, bool isWriteable)
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
		page->User = !isKernel;
		page->Frame = idx;
	}
}

void Paging::FreePage(Page* page)
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

void Paging::Init()
{
	uint32_t memoryEndPage = 0x1000000;
	
	_nframes = memoryEndPage / 0x1000;
	_frames = (uint32_t*)MemoryManager::Allocate(INDEX_FROM_BIT(_nframes));
	memset(_frames, 0, INDEX_FROM_BIT(_nframes));
	
	_kernelDirectory = (PageDirectory*)MemoryManager::AllocateAligned(sizeof(PageDirectory));
	memset(_kernelDirectory, 0, sizeof(PageDirectory));
	_currentDirectory = _kernelDirectory;
	
	uint32_t i = 0;
	while(i < MemoryManager::GetEndAddress())
	{
		AllocatePage(GetPage(i, 1, _kernelDirectory), 0, 0);
		i += 0x1000;
	}
	
	Interrupts::RegisterInterruptHandler(14, &PageFault);
	
	SwitchPageDirectory(_kernelDirectory);
	
	// enable paging
	uint32_t cr0;
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "r"(cr0));
}

void Paging::SwitchPageDirectory(PageDirectory* directory)
{
	_currentDirectory = directory;
	asm volatile("mov %0, %%cr3":: "r"(&directory->TablesPhysical));
}

Page* Paging::GetPage(uint32_t address, bool make, PageDirectory* directory)
{
	address /= 0x1000;
	uint32_t tableIdx = address / 0x1000;
	if(directory->Tables[tableIdx])
	{
		return &directory->Tables[tableIdx]->Pages[address % 0x1000];
	}
	else if(make)
	{
		uint32_t temp;
		directory->Tables[tableIdx] = (PageTable*)MemoryManager::AllocateAlignedPhysical(sizeof(PageTable), &temp);
		memset(directory->Tables[tableIdx], 0, 0x1000);
		directory->TablesPhysical[tableIdx] = temp | 0x7;
		return &directory->Tables[tableIdx]->Pages[address % 0x1000];
	}
	else
	{
		return 0;
	}
}

void Paging::PageFault(Registers* regs)
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
