#pragma once

#include "BufferBlockADT.h"
#include <cstring>

class BufferBlock : public BufferBlock
{
private:
	static const int BLOCK_SIZE = 4096;
	char buffer[BLOCK_SIZE];
public:
	BufferBlock()
	{
		memset(buffer, 0, BLOCK_SIZE); // allocates buffer with zeros
	}
	
};
