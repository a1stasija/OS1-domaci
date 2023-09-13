//
// Created by os on 7/24/23.
//
#include "../h/MemoryAllocator.hpp"
MemoryAllocator* MemoryAllocator:: memAlloc = nullptr;

MemoryAllocator* MemoryAllocator::getMemAlloc(){
    if(!memAlloc){
        memAlloc = (MemoryAllocator*) HEAP_START_ADDR;
        memAlloc->freeMemHead = (BlockHeader*) ((char*)HEAP_START_ADDR + sizeof(MemoryAllocator));
        memAlloc->takenMemHead= nullptr;
        memAlloc->freeMemHead->size=(size_t)((char*)HEAP_END_ADDR-(char*)memAlloc->freeMemHead- sizeof(BlockHeader));
        memAlloc->freeMemHead->next= nullptr;
    }
    return memAlloc;
}

void* MemoryAllocator::kmem_alloc(size_t size) {
    if(size==0 || !freeMemHead) return nullptr;
    //nadji first fit slobodan blok
    BlockHeader *blk=freeMemHead,*prev= nullptr;
    while(blk && blk->size < size){
        prev=blk;
        blk = blk->next;
    }
    if(blk== nullptr){
        //nema slobodnog prostora
        return nullptr;
    }
    //alociraj na pronadjenom bloku
    size_t remaining=blk->size-size;
    if(remaining<=sizeof(BlockHeader)+MEM_BLOCK_SIZE){
        //nedovoljno prostora ostaje za fragment, zauzmi ceo blok
        if(prev) prev->next=blk->next;
        else freeMemHead=blk->next;
    }else{
        //ostaje fragment
        blk->size=size; //stvarni zauzet prostor bez sizeof
        size_t offset= sizeof(BlockHeader)+size;
        BlockHeader* newBlk=(BlockHeader*)((char*)blk+offset);
        if(prev) prev->next=newBlk;
        else freeMemHead=newBlk;
        newBlk->next=blk->next;
        newBlk->size=remaining- sizeof(BlockHeader);
    }
    if(!takenMemHead || (char*)blk<(char*)takenMemHead){
        blk->next=takenMemHead;
        takenMemHead=blk;
    }
    else{
        BlockHeader* cur= takenMemHead;
        while (cur->next != nullptr && (char*)cur->next<(char*)blk) {
            cur = cur->next;
        }
        blk->next = cur->next;
        cur->next = blk;
    }
    return (char*)blk+sizeof(BlockHeader);
}

int MemoryAllocator::kmem_free(void *addr) {
    if(!addr) return -1;
    if(((char*)addr-(char*)((char*)(takenMemHead)+sizeof(BlockHeader)))<0 || ((char*)HEAP_END_ADDR-(char*)addr)<0) return -2;
    BlockHeader* newBlk=(BlockHeader*)((char*)addr-sizeof(BlockHeader));
    //nadji mesto newBlk-a u listi slobodnih
    BlockHeader* cur=freeMemHead;
    BlockHeader* pom= nullptr;
    if(!freeMemHead ||(char*)addr<(char*)freeMemHead){
        pom=newBlk->next;
        newBlk->next=freeMemHead;
        freeMemHead=newBlk;
    } else{
        while (cur->next != nullptr && (char*)cur->next<(char*)addr) {
            cur = cur->next;
        }
        newBlk->next=cur->next;
        cur->next=newBlk;
    }
    //nadji velicinu i apdejtuj listu zauzetih
    BlockHeader* cur1=takenMemHead;
    BlockHeader* prev= nullptr;
    addr=(BlockHeader*)((char*)addr- sizeof(BlockHeader));
    if(!takenMemHead ||(char*)addr<(char*)takenMemHead){
        return -3;
    }

    while (cur1->next != nullptr && (char*)cur1->next<(char*)addr) {
        prev=cur1;
        cur1 = cur1->next;
    }

    if(cur1==takenMemHead){
        takenMemHead= cur1->next;
    }else{
        prev->next=pom;
    }

    newBlk->size=cur1->size;


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