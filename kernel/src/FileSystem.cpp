#include "FileSystem.h"

uint32_t FileSystem::Read(FileSystemNode* node, uint32_t offset, uint32_t size, uint8_t* buffer)
{
	if(node->Read != 0)
	{
		return node->Read(node, offset, size, buffer);
	}
	
	return 0;
}

uint32_t FileSystem::Write(FileSystemNode* node, uint32_t offset, uint32_t size, uint8_t* buffer)
{
	if(node->Write != 0)
	{
		return node->Write(node, offset, size, buffer);
	}
	
	return 0;
}

void FileSystem::Open(FileSystemNode* node, bool read, bool write)
{
	if(node->Open != 0)
	{
		node->Open(node, read, write);
	}
}

void FileSystem::Close(FileSystemNode* node)
{
	if(node->Close != 0)
	{
		node->Close(node);
	}
}

FileSystemNode* FileSystem::ReadDirectory(FileSystemNode* node, uint32_t index)
{
	if((node->Flags & 0x07) == FS_DIRECTORY && node->ReadDirectory != 0)
	{
		return node->ReadDirectory(node, index);
	}
	
	return NULL;
}

FileSystemNode* FileSystem::FindDirectory(FileSystemNode* node, char* name)
{
	if((node->Flags & 0x07) == FS_DIRECTORY && node->FindDirectory != 0)
	{
		return node->FindDirectory(node, name);
	}
	
	return NULL;
}
