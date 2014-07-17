#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include "kernel/InitrdDef.h"

int main(int argc, char** argv)
{
	uint32_t headerCount = (argc - 1) / 2;
	InitrdFileHeader* headers = new InitrdFileHeader[headerCount];
	uint32_t offset = sizeof(InitrdFileHeader) * headerCount + sizeof(uint32_t);
	
	for(uint32_t i = 0; i < headerCount; ++i)
	{
		printf("Writing file %s->%s at 0x%x\n", argv[i * 2 + 1], argv[i * 2 + 2], offset);
		strcpy(headers[i].Name, argv[i * 2 + 2]);
		headers[i].Offset = offset;
		FILE* stream = fopen(argv[i * 2 + 1], "r");
		if(stream == 0)
		{
			printf("Error: file not found: %s\n", argv[i * 2 + 1]);
			return 1;
		}
		
		fseek(stream, 0, SEEK_END);
		headers[i].Length = ftell(stream);
		offset += headers[i].Length;
		fclose(stream);
	}
	
	FILE* wstream = fopen("./initrd.img", "w");
	uint8_t* data = (uint8_t*)malloc(offset);
	fwrite(&headerCount, sizeof(uint32_t), 1, wstream);
	fwrite(headers, sizeof(InitrdFileHeader), headerCount, wstream);
	
	for(uint32_t i = 0; i < headerCount; ++i)
	{
		FILE* stream = fopen(argv[i * 2 + 1], "r");
		uint8_t* buffer = (uint8_t*)malloc(headers[i].Length);
		fread(buffer, 1, headers[i].Length, stream);
		fwrite(buffer, 1, headers[i].Length, wstream);
		fclose(stream);
		free(buffer);
	}
	
	fclose(wstream);
	free(data);
	
	return 0;
}
