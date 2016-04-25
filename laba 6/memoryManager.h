#pragma once
#include <atlcore.h>

// sizeof pool
const int maxPoolSize = 4096;

struct MManagerBlock {
	MManagerBlock *mmbNext;
	void *mmbHeapPool;

protected:
	inline void * __cdecl operator new(size_t n){ n; }
	inline void   __cdecl operator delete(void *p) { p; }
};

struct MManagerExtent {
	MManagerExtent *mmeNext;
	MManagerBlock *mmeCurrent;
	MManagerBlock mmeBlocksArray[0];

protected:
	inline void * __cdecl operator new(size_t n){ n; }
	inline void   __cdecl operator delete(void* p) { p; }
};

inline void * HeapInternalAlloc(size_t size) {
	void *p = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, size);
	return p;
}

inline void HeapInternalFree(void *pBlock) {
	HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, pBlock);
}

class MManagerHeapPool {
public:
	inline void * __cdecl operator new(size_t n) {
		return HeapInternalAlloc(n);
	}

	inline void __cdecl operator delete(void *p) {
		HeapInternalFree(p);
	}

	void * Alloc() {
		MManagerBlock *p = AllocBlock();
		p++; // offset on the block
		return p;
	}

	MManagerHeapPool(size_t BlockSize, // block size in pool
		int initBlockNum = 0) // initial number of blocks
		: currExtent(NULL), freeListOfBlocks(NULL) {
		// real block size
		enterBlockSize = sizeof(MManagerBlock) + BlockSize;
		// if initial number of blocks more then null
		if (initBlockNum > 0) {
			AddExtent(initBlockNum); // new extent
		}
	}

	~MManagerHeapPool(void) {
		MManagerExtent *pExtent = currExtent;
		while (pExtent) {
			MManagerExtent *pNext = pExtent->mmeNext;
			HeapInternalFree(pExtent);
			pExtent = pNext;
		}
	}

	__forceinline MManagerBlock * AllocBlock() {
		// if list of free block is not empty
		if (freeListOfBlocks) {
			// take first and move to user
			register MManagerBlock *pBlock = freeListOfBlocks;
			freeListOfBlocks = freeListOfBlocks->mmbNext;
			return pBlock;
		}
		else {
			return GetBlock(); // take new block form this extent
		}
	}

	__forceinline void FreeBlock(MManagerBlock * pBlock) {
		// add block to list of free blocks
		pBlock->mmbNext = freeListOfBlocks;
		freeListOfBlocks = pBlock;
	}

protected:
	size_t currentExtentSize; // number of blocks in this extend
	size_t enterBlockSize; // inside block size
	MManagerExtent *currExtent; // this extend of pool
	MManagerBlock *freeListOfBlocks; // list of free blocks

	void AddExtent(size_t blockCurrent) {
		ATLASSERT(!currExtent || currExtent->mmeCurrent
			== currExtent->mmeBlocksArray);

		// data for new extend
		MManagerExtent *pExtent = (MManagerExtent*)HeapInternalAlloc(
			sizeof(MManagerExtent) + (blockCurrent + 1) * enterBlockSize);
		pExtent->mmeNext = currExtent;
		// the last block pointer
		pExtent->mmeCurrent = (MManagerBlock*)((BYTE*)(pExtent->mmeBlocksArray)
			+ blockCurrent * enterBlockSize);

		// add new extend to other extends
		currExtent = pExtent;
		// size of this extend for next...
		currentExtentSize = blockCurrent;
	}

	__forceinline MManagerBlock * GetBlock() {
		// is full or not this extend
		if (currExtent->mmeCurrent == currExtent->mmeBlocksArray) {
			AddExtent(currentExtentSize * 2);
		}

		MManagerBlock *pReturn = currExtent->mmeCurrent;
		// Initialize the title of the new block
		pReturn->mmbHeapPool = this;

		// reduce the pointer to the current block
		(BYTE*&)currExtent->mmeCurrent -= enterBlockSize;
		return pReturn;
	}
};

class MManagerHeap {
public:
	// array of pools which contains pointer of pool whith size of block
	MManagerHeapPool *poolArray[1024];

	inline void * __cdecl operator new(size_t n) {
		return HeapInternalAlloc(n);
	}
	
	inline void __cdecl operator delete(void* p) {
		HeapInternalFree(p);
	}

	MManagerHeap() {
		// initialize mass of pool
		const int iSisze = sizeof(poolArray);
		ZeroMemory(poolArray, iSisze);
	}

	~MManagerHeap() {
		for (int i = 0; i < sizeof(poolArray) / sizeof(poolArray[0]); i++) {
			delete(poolArray[i]);
		}
	}

	// new block size of...
	__forceinline void * Alloc(size_t neededBlockSize) {
		const int maxHeapSize = (sizeof(poolArray) / sizeof(poolArray[0]));
		// if size of block more than count elements in mass
		if (neededBlockSize >= maxHeapSize) {
			MManagerBlock *pBlock = (MManagerBlock*)HeapInternalAlloc(
				sizeof(MManagerHeapPool) + neededBlockSize);

			// pointer for heap
			pBlock->mmbHeapPool = (MManagerHeapPool*) - 1;
			++pBlock;
			return pBlock;
		}
		else if (neededBlockSize <= 0) {
			return NULL;
		}
		else {
			// if block is small, than new data in pool
			// pointer for pool of current size
			MManagerHeapPool * pCurrentPool = poolArray[neededBlockSize];

			// if pool is empty
			if (!pCurrentPool) {
				// create new pool and move it to mass
				pCurrentPool = poolArray[neededBlockSize] =
					new MManagerHeapPool(neededBlockSize, maxPoolSize / (int)neededBlockSize);
			}
			// block form pool
			return pCurrentPool->Alloc();
		}
	}

	__forceinline void Free(void * p) {
		// pointer of block start
		MManagerBlock *pBlock = (MManagerBlock *)p;
		pBlock--;
		// heap which was occupied by a block
		MManagerHeapPool *pHeapPool = (MManagerHeapPool*)pBlock->mmbHeapPool;

		// if first
		if ((MManagerHeapPool*) - 1 == pHeapPool) {
			HeapInternalFree(pBlock);
		}
		else {
			pHeapPool->FreeBlock(pBlock);
		}
	}
};