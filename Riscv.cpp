
#include "../h/Riscv.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/PCB.hpp"
#include "../lib/console.h"

void Riscv::handleBaseTrap(){

    uint64 scause = r_scause();
    if (scause== 0x0000000000000008UL || scause== 0x0000000000000009UL){
        void* ret= nullptr;
        //granjanje na sistemske
        int code;
        __asm__ volatile("mv %0,a0":"=r"(code));
        switch (code) {
            case (0x01):{ //mem_alloc
                size_t num;
                __asm__ volatile("mv %0,a1":"=r"(num));
                size_t size = num * MEM_BLOCK_SIZE;
                MemoryAllocator* memoryAllocator= MemoryAllocator::getMemAlloc();
                ret = memoryAllocator->kmem_alloc(size);
                __asm__ volatile("mv a0,%0"::"r"(ret));
                uint64 volatile sepc = Riscv::r_sepc()+4;
                Riscv::w_sepc(sepc);
                break;
        }
            case(0x02): { //mem_free
                void *free;
                __asm__ volatile("mv %0, a1":"=r"(free));
                MemoryAllocator* memoryAllocator= MemoryAllocator::getMemAlloc();
                int ret = memoryAllocator->kmem_free(free);
                __asm__ volatile("mv a0,%0"::"r"(ret));
                uint64 volatile sepc = Riscv::r_sepc()+4;
                Riscv::w_sepc(sepc);
                break;
            }
            case(0x11):{//thread_create
                PCB** handle;
                __asm__ volatile ("mv %0, a1":"=r"(handle));
                PCB::Body start;
                __asm__ volatile("mv %0, a2":"=r"(start));
                void* arg;
                __asm__ volatile("mv %0, a6":"=r"(arg));
                void* stk;
                __asm__ volatile("mv %0, a7":"=r"(stk));
                *handle=PCB::createThread(start,arg, (uint64*)stk,false);
                int ret;
                if(!handle){
                    ret=-1;
                }else{
                    ret=0;
                }
                __asm__ volatile("mv a0,%0"::"r"(ret));
                uint64 volatile sepc = Riscv::r_sepc()+4;
                Riscv::w_sepc(sepc);
                break;
            }
            case(0x12):{ //thread_exit
                int ret = 0;
                PCB::running->setFinished(true);
                PCB::wake(PCB::running);
                uint64 volatile sepc = r_sepc() + 4;
                uint64 volatile sstatus = r_sstatus();
                PCB::kdispatch();
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            case(0x13): { //thread_dispatch
                uint64 volatile sepc = r_sepc() + 4;
                uint64 volatile sstatus = r_sstatus();
                PCB::kdispatch();
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            case(0x14):{ //thread_join
                PCB* p;
                __asm__ volatile ("mv %0, a1":"=r"(p));
                PCB::kjoin(PCB::running, p);
                PCB::running->suspended=true;
                uint64 volatile sepc = r_sepc() + 4;
                uint64 volatile sstatus = r_sstatus();
                PCB::kdispatch();
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            case(0x21):{ //sem_open
                int ret=0;
                KSemaphore** handle;
                __asm__ volatile ("mv %0, a1":"=r"(handle));
                unsigned init;
                __asm__ volatile("mv %0, a2":"=r"(init));
                *handle=KSemaphore::createKSemaphore(init);
                if(!*handle){
                    ret=-1;
                }
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                uint64 volatile sepc = Riscv::r_sepc()+4;
                Riscv::w_sepc(sepc);
                break;
            }
            case(0x22):{ //sem_close
               int ret=0;
               KSemaphore * sem;
               __asm__ volatile ("mv %0, a1":"=r"(sem));
               if(!sem){
                    ret=-1;
               }
               sem->~KSemaphore();
               __asm__ volatile("mv a0, %0" : : "r" (ret));
                uint64 volatile sepc = Riscv::r_sepc()+4;
                Riscv::w_sepc(sepc);
               break;
            }
            case(0x23):{ //sem_wait
                int ret=0;
                KSemaphore* sem;
                __asm__ volatile ("mv %0, a1":"=r"(sem));
                if(!sem){
                    ret=-2;
                }
                else {
                    PCB* pom=PCB::running;
                    uint64 volatile sepc = r_sepc() + 4;
                    uint64 volatile sstatus = r_sstatus();
                    sem->wait();
                    w_sstatus(sstatus);
                    w_sepc(sepc);
                    if (pom->error) {
                        ret = -1;
                    }
                }
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case(0x24):{ //sem_signal
                int ret;
                KSemaphore * sem;
                __asm__ volatile ("mv %0, a1":"=r"(sem));
                if(!sem){
                    ret=-3;
                }
                ret=sem->signal();
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                uint64 volatile sepc = Riscv::r_sepc()+4;
                Riscv::w_sepc(sepc);
                break;
            }
            case(0x25):{
                uint64 volatile sepc = Riscv::r_sepc()+4;
                char a=__getc();
                __asm__ volatile("mv a0, %0" : : "r" (a));
                Riscv::w_sepc(sepc);
                break;
            }
        }
        mc_sip(SIP_SSIP);
    }else if((scause&8000000000000000)==0){

    }
    else{
    }

}

void Riscv::popSppSpie() {
    uint64 volatile sstatus;
    sstatus = r_sstatus();
    if(PCB::running->kernelThread){
        sstatus = sstatus | SSTATUS_SPP;
    }
    else{
        sstatus = sstatus & ~SSTATUS_SPP;
    }
    w_sstatus(sstatus);
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}
void Riscv::vectorTable(void) {
    __asm__ volatile (
            ".org  _ZN5Riscv11vectorTableEv + 0*4;"
            "jal   x0,_ZN5Riscv8baseTrapEv;"  /* 0 ecall and exceptions */
            ".org  _ZN5Riscv11vectorTableEv + 1*4;"
            "jal   x0,_ZN5Riscv9timerTrapEv;"  /* 1  timer*/
            ".org  _ZN5Riscv11vectorTableEv + 9*4;"
            "jal   x0,_ZN5Riscv11consoleTrapEv;" ::: /* 9 console */ );
}

void Riscv::handleTimerTrap() {
        mc_sip(SIP_SSIP);
}

void Riscv::handleConsoleTrap() {
    console_handler();
    //mc_sip(SIP_SSIP);
}