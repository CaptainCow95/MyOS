#ifndef PAGING_H
#define PAGING_H

#include "Common.h"
#include "Interrupts.h"

struct Page
{
	uint32_t Present : 1;
	uint32_t ReadWrite : 1;
	uint32_t User : 1;
	uint32_t Accessed : 1;
	uint32_t Dirty : 1;
	uint32_t Unused : 7;
	uint32_t Frame : 20;
};

struct PageTable
{
	Page Pages[1024];
};

struct PageDirectory
{
	PageTable* Tables[1024];
	uint32_t TablesPhysical[1024];
	uint32_t PhysicalAddress;
};

class Paging
{
public:
	static void Init();
	static void SwitchPageDirectory(PageDirectory*);
	static Page* GetPage(uint32_t, bool, PageDirectory*);
	static void AllocatePage(Page*, bool, bool);
	static void FreePage(Page*);
private:
	static void PageFault(Registers*);
	
	static void SetFrame(uint32_t);
	static void ClearFrame(uint32_t);
	static uint32_t TestFrame(uint32_t);
	static uint32_t FirstFrame();
	
	static uint32_t* _frames;
	static uint32_t _nframes;
	static PageDirectory* _kernelDirectory;
	static PageDirectory* _currentDirectory;
};

#endif
