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

	void getBytes(char* space, int size, int position) override {
		if (!file.is_open() || space == nullptr || size <= 0 || position < 0) return;

		int bytesRemaining = size;
		int writeOffset = 0;
		int filePos = position;

		while (bytesRemaining > 0) {
			int blockNum = filePos / blockSize;
			int offsetInBlock = filePos % blockSize;

			int bufIndex = findBufferIndex(blockNum);
			if (bufIndex == -1) {
				// Block not in pool -- evict LRU and load from disk
				bufIndex = evictLRUBuffer();
				loadBlockFromDisk(blockNum, bufIndex);
			}

			// Move accessed block to most-recently-used position
			moveIndexToFront(bufIndex);

			// How many bytes available in this block from the current offset
			int available = blockSize - offsetInBlock;
			int toCopy = min(bytesRemaining, available);

			pool[bufIndex]->getData(offsetInBlock, toCopy, space + writeOffset);

			bytesRemaining -= toCopy;
			writeOffset += toCopy;
			filePos += toCopy;
		}
	}

	void printBufferBlockOrder() override {
		cout << "My buffer block order from most recently used to LRU is:\n\t";
		for (int index : lruOrder) {
			cout << pool[index]->getID() << ", ";
		}
		cout << "\n";
	}

	int getLRUBlockID() override {
		if (lruOrder.empty()) {
			return -1;
		}
		int lruIndex = lruOrder.back();
		return pool[lruIndex]->getID();
	}

private:
	string filename;
	ifstream file;
	int poolSize;
	int blockSize;
	vector<BufferBlock*> pool;
	list<int> lruOrder;                // Front = MRU, back = LRU
	unordered_map<int, int> idToIndex; // blockID -> pool slot index

	// Returns pool slot index for the given block ID, or -1 if not cached
	int findBufferIndex(int id) {
		auto it = idToIndex.find(id);
		if (it == idToIndex.end()) {
			return -1;
		}
		return it->second;
	}

	// Evicts the LRU block and returns its pool slot index for reuse
	int evictLRUBuffer() {
		if (lruOrder.empty()) {
			return 0;
		}
		int index = lruOrder.back();
		lruOrder.pop_back();

		int oldID = pool[index]->getID();
		idToIndex.erase(oldID);

		return index;
	}

	// Moves a pool slot index to the front (MRU position) of lruOrder
	void moveIndexToFront(int index) {
		auto it = find(lruOrder.begin(), lruOrder.end(), index);
		if (it != lruOrder.end()) {
			lruOrder.erase(it);
		}
		lruOrder.push_front(index);
	}
};