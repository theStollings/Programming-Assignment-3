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
	char* buffer; // pointer to block buffer (after the 4-byte header)
	int BLOCK_SIZE; // total size of the block (including header)
public:
	BufferBlock()
	{
		memset(buffer, 0, BLOCK_SIZE); // allocates buffer with zeros
	}

	BufferBlock(char* initData = nullptr, int size = BLOCKSIZE) {
		BLOCK_SIZE = size;
		buffer = new char[BLOCK_SIZE];
		memset(buffer, 0, BLOCK_SIZE);

		blockID = -1;
		int32_t id = -1; // default block ID for uninitialized block
		memcpy(buffer, &id, sizeof(id)); // store default block ID in first 4 bytes

		if (initData != nullptr) {
			memcpy(buffer, initData, BLOCK_SIZE); // copy initData into the block (including header)
			int32_t stored = 0;
			memcpy(&stored, buffer, sizeof(stored)); // read block ID from first 4 bytes
			blockID = static_cast<int>(stored); // cache block ID in member variable
		}
	}

	virtual ~BufferBlock() { // make sure child destructor runs
		delete[] buffer;
		buffer = nullptr;
	}

	virtual void getData(int pos, int sz, char* outData)
	{
		if (outData == nullptr || pos < 0 || sz <= 0) {
			return;
		}

		const int header = static_cast<int>(sizeof(int32_t)); // 4 bytes for block ID
		int bufferRegionSize = BLOCK_SIZE - header;

		if (pos >= bufferRegionSize) {
			return;
		}

		int available = bufferRegionSize - pos;
		int toCopy = std::min(sz, available); // how many bytes we can actually copy from the buffer region
		if (toCopy > 0) {
			std::memcpy(outData, buffer + header + pos, toCopy); // copy from buffer region, skipping header
		}
	}

	virtual void setID(int id) override
	{
		blockID = id;
		int32_t id32 = static_cast<int32_t>(id); // store the block ID as a 4-byte integer in the first 4 bytes of buffer
		memcpy(buffer, &id32, sizeof(id32)); // copy the block ID into the first 4 bytes of the buffer buffer
	}

	virtual int getID() const override
	{
		int32_t id32 = 0;
		memcpy(&id32, buffer, sizeof(id32)); // read the block ID from the first 4 bytes of the buffer buffer
		return static_cast<int>(id32); // return the block ID as an int
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
		if (blk == nullptr) {
			return;
		}
		memcpy(buffer, blk, BLOCK_SIZE);
		int32_t stored = 0;
		memcpy(&stored, buffer, sizeof(stored));
		blockID = static_cast<int>(stored);
	}
};