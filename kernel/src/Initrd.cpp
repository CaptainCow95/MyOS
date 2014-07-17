#include "Initrd.h"
#include "MemoryManager.h"

InitrdHeader* Initrd::_header;
InitrdFileHeader* Initrd::_fileHeaders;
FileSystemNode* Initrd::_root;
FileSystemNode* Initrd::_nodes;
uint32_t Initrd::_nodeCount;

FileSystemNode* Initrd::Init(uint32_t location)
{
	_header = (InitrdHeader*)location;
	_fileHeaders = (InitrdFileHeader*)((uint32_t)_header + sizeof(InitrdHeader));
	
	_root = (FileSystemNode*)MemoryManager::Allocate(sizeof(FileSystemNode));
	strcpy(_root->Name, "initrd");
	_root->Inode = 0;
	_root->Length = 0;
	_root->Flags = FS_DIRECTORY;
	_root->Read = 0;
	_root->Write = 0;
	_root->Open = 0;
	_root->Close = 0;
	_root->ReadDirectory = &ReadDirectory;
	_root->FindDirectory = 0;
	_root->Ptr = 0;
	_root->Implementation = 0;
	
	_nodes = (FileSystemNode*)MemoryManager::Allocate(sizeof(FileSystemNode) * _header->FileCount);
	_nodeCount = _header->FileCount;
	
	for(uint32_t i = 0; i < _nodeCount; ++i)
	{
		_fileHeaders[i].Offset += location;
		
		strcpy(_nodes[i].Name, (const char*)&_fileHeaders[i].Name);
		_nodes[i].Length = _fileHeaders[i].Length;
		_nodes[i].Inode = i;
		_nodes[i].Flags = FS_FILE;
		_nodes[i].Read = &Read;
		_nodes[i].Write = 0;
		_nodes[i].ReadDirectory = 0;
		_nodes[i].FindDirectory = 0;
		_nodes[i].Open = 0;
		_nodes[i].Close = 0;
		_nodes[i].Implementation = 0;
	}
	
	return _root;
}

uint32_t Initrd::Read(FileSystemNode* node, uint32_t offset, uint32_t size, uint8_t* buffer)
{
	InitrdFileHeader header = _fileHeaders[node->Inode];
	if(offset > header.Length)
	{
		return 0;
	}
	
	if(offset + size > header.Length)
	{
		size = header.Length - offset;
	}
	
	memcpy(buffer, (uint8_t*)(header.Offset + offset), size);
	return size;
}

FileSystemNode* Initrd::ReadDirectory(FileSystemNode* node, uint32_t index)
{
	if(node == _root)
	{
		if(index < _nodeCount)
		{
			return &_nodes[index];
		}
		
		return 0;
	}
	
	return 0;
}
