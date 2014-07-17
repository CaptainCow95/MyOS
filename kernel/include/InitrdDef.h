#ifndef INITRDDEF_H
#define INITRDDEF_H

#include <stdint.h>

struct InitrdHeader
{
	uint32_t FileCount;
};

struct InitrdFileHeader
{
	char Name[128];
	uint32_t Offset;
	uint32_t Length;
};

#endif
