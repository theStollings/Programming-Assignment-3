#pragma once

#include "BufferBlockADT.h"
#include <cstring> // for memcpy
#include <cstdint>

class BufferBlock : public BufferBlockADT
{
private:
	static const int BLOCK_SIZE = 4096;
	char buffer[BLOCK_SIZE];
public:
	BufferBlock()
	{
		memset(buffer, 0, BLOCK_SIZE); // allocates buffer with zeros
	}

	BufferBlock(char* data, int sz = 4096)
	{
		memset(buffer, 0, BLOCK_SIZE);
		if (data != nullptr)
		{
			int copySize = (sz < BLOCK_SIZE) ? sz : BLOCK_SIZE; // copy-size within bounds
			memcpy(buffer, data, copySize); // copy data to buffer
		}
	}

	virtual ~BufferBlock() {} // make sure child destructor runs

	virtual void setID(int id) override
	{
		memcpy(buffer, &id, sizeof(int32_t)); // first 4 bytes to id
	}



};