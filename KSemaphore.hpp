
#ifndef PROJECT_BASE_V1_1_KSEMAPHORE_HPP
#define PROJECT_BASE_V1_1_KSEMAPHORE_HPP

#include "../lib/hw.h"
#include "../h/PCB.hpp"
class PCB;

class KSemaphore{
public:
    KSemaphore(int val=1);
    virtual ~KSemaphore();
    void wait();
    int signal();
    static KSemaphore* createKSemaphore(int v);
    void* operator new(size_t);
    void operator delete(void*p);
    void* operator new[](size_t);
    void operator delete[](void*p);
private:
    int value;
    PCB* head;
    PCB* tail;
};
#endif //PROJECT_BASE_V1_1_KSEMAPHORE_HPP
