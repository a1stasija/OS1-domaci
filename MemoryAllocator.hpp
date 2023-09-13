//
// Created by os on 7/24/23.
//

#ifndef PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
#include "../lib/hw.h"

class MemoryAllocator {
private:
    MemoryAllocator(){}
    static MemoryAllocator* memAlloc;
    struct BlockHeader{
        BlockHeader* next;
        size_t size;
    };

    BlockHeader* freeMemHead;
    BlockHeader* takenMemHead;
    static void joinBlocks(BlockHeader* blk);

public:
    MemoryAllocator(const MemoryAllocator& obj)=delete;
    static MemoryAllocator* getMemAlloc();
    void* kmem_alloc(size_t);
    int kmem_free(void*);
};
#endif //PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP