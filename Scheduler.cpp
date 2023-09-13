//
// Created by os on 7/28/23.
//
#include "../h/Scheduler.hpp"
#include "../h/PCB.hpp"

Scheduler* Scheduler::scheduler = nullptr;


PCB* Scheduler::get(){
    Scheduler* s=getScheduler();
    PCB* pom=s->head;
    s->head=s->head->nextPCB;
    if (!s->head) s->tail = nullptr;
    pom->nextPCB= nullptr;
    return pom;
}
void Scheduler::put(PCB *p) {
    Scheduler* s=getScheduler();
    if (s->tail) {
        s->tail->nextPCB = p;
        s->tail = s->tail->nextPCB;
    } else {
        s->head = s->tail = p;
    }
}

Scheduler* Scheduler::getScheduler() {
    if (!scheduler){
        MemoryAllocator* m=MemoryAllocator::getMemAlloc();
        scheduler=(Scheduler*)m->kmem_alloc(sizeof(Scheduler));
        scheduler->head= nullptr;
        scheduler->tail= nullptr;
    }
    return scheduler;
}
