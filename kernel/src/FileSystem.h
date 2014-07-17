#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Common.h"

#define FS_FILE 0x01
#define FS_DIRECTORY 0x02
#define FS_CHARDEVICE 0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE 0x05
#define FS_SYMLINK 0x06
#define FS_MOUNTPOINT 0x08

struct DirectoryEntry;
struct FileSystemNode;

typedef uint32_t (*ReadType)(struct FileSystemNode*, uint32_t, uint32_t, uint8_t*);
typedef uint32_t (*WriteType)(struct FileSystemNode*, uint32_t, uint32_t, uint8_t*);
typedef void (*OpenType)(struct FileSystemNode*, bool, bool);
typedef void (*CloseType)(struct FileSystemNode*);
typedef FileSystemNode* (*ReadDirectoryType)(struct FileSystemNode*, uint32_t);
typedef FileSystemNode* (*FindDirectoryType)(struct FileSystemNode*, char* name);

struct FileSystemNode
{
	char Name[128];
	uint32_t Flags;
	uint32_t Inode;
	uint32_t Length;
	uint32_t Implementation;
	ReadType Read;
	WriteType Write;
	OpenType Open;
	CloseType Close;
	ReadDirectoryType ReadDirectory;
	FindDirectoryType FindDirectory;
	FileSystemNode* Ptr;
};

class FileSystem
{
public:
	static uint32_t Read(FileSystemNode*, uint32_t, uint32_t, uint8_t*);
	static uint32_t Write(FileSystemNode*, uint32_t, uint32_t, uint8_t*);
	static void Open(FileSystemNode*, bool, bool);
	static void Close(FileSystemNode*);
	static FileSystemNode* ReadDirectory(FileSystemNode*, uint32_t);
	static FileSystemNode* FindDirectory(FileSystemNode*, char*);
};

#endif
