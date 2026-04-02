#pragma once

#include "BufferPoolADT.h"
#include "BufferBlock.h"
#include "constants.h"

#include <string> // for std::string
#include <fstream> // for std::ifstream
#include <vector> // for std::vector
#include <list> // for std::list to maintain LRU order
#include <unordered_map> // for mapping block IDs to pool indices
#include <algorithm> // for std::min
#include <iostream> // for std::cout, std::cerr
#include <cstdint> // for fixed-width integer types (e.g., int32_t)
#include <cstring> // for std::memcpy

class LRUBufferPool : public BufferPoolADT {
public:
	LRUBufferPool(const string& filename, int poolSize = POOL_SIZE, int blockSize = BLOCKSIZE)
		: filename(filename), poolSize(poolSize), blockSize(blockSize) {

		file.open(filename, std::ios::in | std::ios::binary);
		if (!file.is_open()) {
			cerr << "Failed to open file: " << filename << endl;
			return;
		}

		pool.resize(poolSize, nullptr);
		for (int i = 0; i < poolSize; ++i) {
			pool[i] = new BufferBlock(nullptr, blockSize);
			loadBlockFromDisk(i, i);   // Load block i into pool slot i
			lruOrder.push_back(i);     // Initial order: 0, 1, 2, 3, 4
			idToIndex[i] = i;
		}
	}

	~LRUBufferPool() override {
		if (file.is_open()) {
			file.close();
		}
		for (auto block : pool) {
			delete block;
		}
		pool.clear();
	}

private:
	string filename;
	ifstream file;
	int poolSize;
	int blockSize;
	vector<BufferBlock*> pool;
	list<int> lruOrder;                // Front = MRU, back = LRU
	unordered_map<int, int> idToIndex; // blockID -> pool slot index
};