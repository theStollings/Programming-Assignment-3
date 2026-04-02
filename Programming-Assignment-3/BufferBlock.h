#pragma once

#include "BufferBlockADT.h"
#include "constants.h"

#include <cstring> // for memcpy, copying raw bytes of memory
#include <cstdint> // precisley 4 bytes for int, not machine dependent
#include <algorithm> // for std::min

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

	virtual void getData(int pos, int sz, char* data)
	{
		// CHECK FOR VALID INPUTS! MORE WORK ON THIS
		int availableBytes = BLOCK_SIZE - pos; // get available bytes
		int copySize = (sz < availableBytes) ? sz : availableBytes; // valid copy size
		memcpy(data, buffer + pos, copySize); // read the range of the block
	}

	virtual void setID(int id) override
	{
		memcpy(buffer, &id, sizeof(int32_t)); // set first 4 bytes to block ID
	}

	virtual int getID() const override
	{
		int id;
		memcpy(&id, buffer, sizeof(int32_t)); // get first 4 bytes (block ID)
		return id;
	}

	inline virtual int getBlockSize() const
	{
		return BLOCK_SIZE;
	}

	virtual char* getBlock() const
	{
		return const_cast<char*>(buffer); // buffer must be cast to const
	}

	virtual void setBlock(char* blk)
	{
		memcpy(buffer, blk, BLOCK_SIZE);
	}
};