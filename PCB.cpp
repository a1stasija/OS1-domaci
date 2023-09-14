//
// Created by os on 7/28/23.
//

#include "../h/PCB.hpp"



PCB *PCB::running= nullptr;
uint64 PCB::timeSliceCounter=0;

PCB::PCB(Body b,void* a,uint64* s, bool kernel){
    body=b;
    stack=b!= nullptr?s:0;
    context.ra=(uint64) &threadWrapper;
    context.sp=(b!= nullptr?(uint64)&stack[2*DEFAULT_STACK_SIZE]: 0);
    finished=false;
    timeSlice=DEFAULT_TIME_SLICE;
    arg=a;
    kernelThread=kernel;
    nextPCB= nullptr;
    sleeping= nullptr;
    error=false;
    suspended=false;
    if(b!= nullptr) {
        Scheduler::put(this);
    }
}
PCB::~PCB() {
        if(body){
            MemoryAllocator* m=MemoryAllocator::getMemAlloc();
            m->kmem_free(stack);
        }
}
PCB *PCB::createThread(PCB::Body body, void* a,uint64* stack,bool kernel) {
    return new PCB(body,a,stack,kernel);

}

void PCB::kdispatch() {
    PCB* old=running;
    if(!old->isFinished() && !old->isSuspended()){
        Scheduler::put(old);

    }
    running=Scheduler::get();
    contextSwitch(&old->context,&running->context);
}


void* PCB::operator new(size_t size) {
    MemoryAllocator* mem=MemoryAllocator::getMemAlloc();
    return mem->kmem_alloc(size);

}
void PCB::operator delete(void *p) {
    MemoryAllocator* mem=MemoryAllocator::getMemAlloc();
    mem->kmem_free(p);
}
void *PCB::operator new[](size_t size) {
    MemoryAllocator* mem=MemoryAllocator::getMemAlloc();
    return mem->kmem_alloc(size);
}


void PCB::operator delete[](void *p) {
    MemoryAllocator* mem=MemoryAllocator::getMemAlloc();
    mem->kmem_free(p);
}

void PCB::wake(PCB* p){
    PCB* pom=p->sleeping;
    while(pom){
        p->sleeping=p->sleeping->nextPCB;
        Scheduler::put(pom);
        pom=p->sleeping;
    }
    p->sleeping= nullptr;
}
void PCB::kjoin(PCB* m,PCB *p) { //
    if(p->isFinished()){
        return;
    }
    m->nextPCB= nullptr;
    PCB* list=p->sleeping;
    if(!list){
        p->sleeping=m;
    }else{
        while(list->nextPCB) {
            list=list->nextPCB;
        }
        list->nextPCB=m;
    }
    return;
}

void PCB::threadWrapper() {
    Riscv::popSppSpie();
    running->body(running->arg);
    thread_exit();
}




