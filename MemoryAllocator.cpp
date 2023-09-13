//
// Created by os on 7/24/23.
//
#include "../h/MemoryAllocator.hpp"
MemoryAllocator* MemoryAllocator:: memAlloc = nullptr;

MemoryAllocator* MemoryAllocator::getMemAlloc(){
    if(!memAlloc){
        memAlloc = (MemoryAllocator*) HEAP_START_ADDR;
        memAlloc->memHead = (BlockHeader*) ((char*) HEAP_START_ADDR + sizeof(MemoryAllocator));
        memAlloc->memHead->next= nullptr;
        memAlloc->memHead->size = (char*) HEAP_END_ADDR - (char*) HEAP_START_ADDR - sizeof(MemoryAllocator) - sizeof(BlockHeader);
        memAlloc->memHead->taken=false;
    }
    return memAlloc;
}
void* MemoryAllocator::kmem_alloc(size_t size) {
    if(size==0 || !memHead) return nullptr;
    size = ((size + MEM_BLOCK_SIZE - 1)/MEM_BLOCK_SIZE) * MEM_BLOCK_SIZE;
    BlockHeader *blk=memHead,*prev= nullptr;
    while(blk && blk->size < size){
        prev=blk;
        blk = blk->next;
    }
    if(blk== nullptr){
        return nullptr;
    }
    size_t remaining=blk->size-size;
    if(remaining<=sizeof(BlockHeader)+MEM_BLOCK_SIZE){
        if(prev) prev->next=blk->next;
        else memHead=blk->next;
    }else{
        blk->size=size;
        size_t offset= sizeof(BlockHeader)+size;
        BlockHeader* newBlk=(BlockHeader*)((char*)blk+offset);
        if(prev) prev->next=newBlk;
        else memHead=newBlk;
        newBlk->next=blk->next;
        newBlk->taken=false;
        newBlk->size=remaining- sizeof(BlockHeader);
    }
    blk->next= nullptr;
    blk->taken=true;
    return (char*)blk+sizeof(BlockHeader);
}

int MemoryAllocator::kmem_free(void *addr) {
    if(!addr) return -1;
    if(((char*)HEAP_END_ADDR-(char*)addr)<0) return -2;
    if((char*)addr < (char*)HEAP_START_ADDR + sizeof(MemoryAllocator) + sizeof(BlockHeader)) return -3;
    BlockHeader* newBlk=(BlockHeader*)((char*)addr-sizeof(BlockHeader));
    if(newBlk->taken==false) return -4;
    BlockHeader* cur=memHead;
    BlockHeader* prev= nullptr;
    if(!memHead ||(char*)addr<(char*)memHead){
        newBlk->next=memHead;
        memHead=newBlk;
    }else{
        while (cur != nullptr && (char*)cur<(char*)addr) {
            prev=cur;
            cur = cur->next;
        }
        newBlk->next=cur;
        prev->next=newBlk;
    }
    newBlk->taken=false;
    joinBlocks(newBlk);
    joinBlocks(cur);
    return 0;

}


void MemoryAllocator::joinBlocks(MemoryAllocator::BlockHeader *blk) {
    if(!blk) return;
    if(blk->next && (char*)((char*)blk+blk->size+sizeof (BlockHeader*))==(char*)(blk->next)){
        blk->size+=blk->next->size;
        blk->next=blk->next->next;
    }
    return;
}