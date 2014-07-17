#ifndef INITRD_H
#define INITRD_H

#include "Common.h"
#include "kernel/InitrdDef.h"
#include "FileSystem.h"

class Initrd
{
public:
	static FileSystemNode* Init(uint32_t);
private:
	static uint32_t Read(FileSystemNode*, uint32_t, uint32_t, uint8_t*);
	static FileSystemNode* ReadDirectory(FileSystemNode*, uint32_t);
	
	static InitrdHeader* _header;
	static InitrdFileHeader* _fileHeaders;
	static FileSystemNode* _root;
	static FileSystemNode* _nodes;
	static uint32_t _nodeCount;
};

#endif
