//
// Created by os on 7/10/23.
//


#include "../h/Riscv.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/syscall_cpp.hpp"
#include "../lib/console.h"
#include "../h/PCB.hpp"


void userMain(void *pVoid);


void main(){
    Riscv::w_stvec((uint64) &(Riscv::vectorTable) | Riscv::STVEC_MODE);
    Scheduler::getScheduler();
    MemoryAllocator* m=MemoryAllocator::getMemAlloc();
    thread_t mainThread=PCB::createThread(nullptr, nullptr, nullptr,true);
    PCB::running=mainThread;
    thread_t userThread=PCB::createThread(&userMain, nullptr, (uint64*)m->kmem_alloc(DEFAULT_STACK_SIZE*sizeof(uint64)),false);

    while(!userThread->isFinished()){
        thread_dispatch();

    }
    delete userThread;

    return;
}




