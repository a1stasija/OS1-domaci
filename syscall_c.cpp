#include "../h/syscall_c.h"
#include "../h/Riscv.hpp"
#include "../h/PCB.hpp"
#include "../lib/console.h"


typedef struct Arg{
    uint64 a0;
    uint64 a1;
    uint64 a2;
    uint64 a3;
    uint64 a4;
}Arg;

void sysCall(void* arg) {
    Arg* a = (Arg*)arg;
    uint64 code = a->a0;
    uint64 a1 = a->a1;
    uint64 a2 = a->a2;
    uint64 a3 = a->a3;
    uint64 a4 = a->a4;
    __asm__ volatile("mv a0, %0" : : "r" (code));
    __asm__ volatile("mv a1, %0" : : "r" (a1));
    __asm__ volatile("mv a2, %0" : : "r" (a2));
    __asm__ volatile("mv a6, %0" : : "r" (a3));
    __asm__ volatile("mv a7, %0" : : "r" (a4));
    __asm__ volatile("ecall");

}

void* mem_alloc(size_t size){
    size_t num=(size+MEM_BLOCK_SIZE-1)/MEM_BLOCK_SIZE;
    Arg args;
    args.a0=0x01;
    args.a1=num;
    sysCall(&args);
    void *ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}

int mem_free(void* addr){
    Arg args;
    args.a0=0x02;
    args.a1=(uint64)addr;
    sysCall(&args);
    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}
void thread_dispatch() {
    Arg args;
    args.a0 = 0x13;
    sysCall(&args);
}

int thread_exit(){
    Arg args;
    args.a0 = 0x12;
    sysCall(&args);
    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}
int thread_create(thread_t* handle, void(*start_routine)(void *), void *arg) {
    MemoryAllocator* m=MemoryAllocator::getMemAlloc();
    uint64* stack= (uint64*)m->kmem_alloc(2*DEFAULT_STACK_SIZE* sizeof(uint64));
    Arg args;
    args.a0 = 0x11;
    args.a1 = (uint64)handle;
    args.a2 = (uint64)start_routine;
    args.a3 = (uint64)arg;
    args.a4=(uint64)stack;
    sysCall(&args);
    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}
void thread_join(thread_t handle){
    if(!handle) return;
    Arg args;
    args.a0 = 0x14;
    args.a1=(uint64)handle;
    sysCall(&args);
}
int sem_open(sem_t* handle, unsigned init){
    Arg args;
    args.a0 = 0x21;
    args.a1=(uint64) handle;
    args.a2=(uint64) init;
    sysCall(&args);
    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}

int sem_close(sem_t handle){
    Arg args;
    args.a0 = 0x22;
    args.a1=(uint64) handle;
    sysCall(&args);
    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}

int sem_wait(sem_t id){
    Arg args;
    args.a0 = 0x23;
    args.a1=(uint64) id;
    sysCall(&args);
    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}

int sem_signal(sem_t id){
    Arg args;
    args.a0 = 0x24;
    args.a1=(uint64) id;
    sysCall(&args);
    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}

int time_sleep(time_t){
    return 0;
}

char getc(){
    Arg args;
    args.a0=0x25;
    sysCall(&args);
    char ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
    //return __getc();
}

void putc(char c){
    return __putc(c);
}
