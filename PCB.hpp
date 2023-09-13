
#ifndef PROJECT_BASE_V1_1_PCB_HPP
#define PROJECT_BASE_V1_1_PCB_HPP
#include "../lib/hw.h"
#include "../h/syscall_c.h"
#include "../h/Scheduler.hpp"
#include "../h/KScheduler.hpp"
#include "../h/Riscv.hpp"
#include "../h/MemoryAllocator.hpp"


class PCB{

public:
    friend class Scheduler;
    friend class KScheduler;
    friend class Riscv;
    friend class KSemaphore;
    ~PCB();
    bool isFinished() const{
        return finished;
    }//implicitno inline
    void setFinished(bool t){
        finished=t;
    }
    bool isSuspended() const{
        return suspended;
    }//implicitno inline
    void setSuspended(bool t){
        suspended=t;
    }
    PCB* getNextPCB(){
        return nextPCB;
    }
    uint64 getTimeSlice() const{
        return timeSlice;
    }
    bool isKernelThread() const{
        return kernelThread;
    }
    bool isError() const{
        return error;
    }
    using Body= void (*)(void*);
    static PCB* createThread(Body body, void* a, uint64* stack, bool kernel);
    static PCB* running;
    void* operator new(size_t);
    void operator delete(void*p);
    void* operator new[](size_t);
    void operator delete[](void*p);
protected:
    bool error;
private:
    PCB(Body b, void* a, uint64* s, bool kernel);
    struct Context{
        uint64 ra;
        uint64 sp;
    };
    Body body;
    uint64* stack;
    Context context;
    bool finished;
    PCB* nextPCB;
    uint64 timeSlice;
    void* arg;
    bool kernelThread;
    bool suspended;
    static uint64 timeSliceCounter;
    static void threadWrapper();
    static void kdispatch();
    static void kjoin(PCB* m,PCB* p);
    static void wake(PCB* p);
    PCB* sleeping; //niti koje cekaju da se ja zavrsim
    static void contextSwitch(PCB::Context *oldContext, PCB::Context *newContext);
};
#endif //PROJECT_BASE_V1_1_PCB_HPP
