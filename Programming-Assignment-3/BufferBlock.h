#pragma once

#include "BufferBlockADT.h"
#include "constants.h"

#include <cstring> // for memcpy, copying raw bytes of memory
#include <cstdint> // precisley 4 bytes for int, not machine dependent
#include <algorithm> // for std::min

class BufferBlock : public BufferBlockADT
{
private:
	int blockID; // block ID stored in first 4 bytes of buffer
	char* data; // pointer to block data (after the 4-byte header)
	int blockSize; // total size of the block (including header)
public:
	BufferBlock()
	{
		memset(data, 0, blockSize); // allocates buffer with zeros
	}

	BufferBlock(char* initData = nullptr, int size = BLOCKSIZE) {
		blockSize = size;
		data = new char[blockSize];
		memset(data, 0, blockSize);

		// Default block ID stored in first 4 bytes
		blockID = -1;
		int32_t id = -1;
		memcpy(data, &id, sizeof(id));

		if (initData != nullptr) {
			memcpy(data, initData, blockSize);
			// Extract block ID stored in first 4 bytes
			int32_t stored = 0;
			memcpy(&stored, data, sizeof(stored));
			blockID = static_cast<int>(stored);
		}
	}

	virtual ~BufferBlock() { // make sure child destructor runs
		delete[] data;
		data = nullptr;
	}

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