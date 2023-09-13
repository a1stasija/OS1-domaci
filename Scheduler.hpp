//
// Created by os on 7/28/23.
//

#ifndef PROJECT_BASE_V1_1_SCHEDULER_HPP
#define PROJECT_BASE_V1_1_SCHEDULER_HPP
#include "../h/syscall_c.h"

class PCB;

class Scheduler{
private:
    Scheduler(){};
    static Scheduler* scheduler;
    PCB* head;
    PCB* tail;
    PCB* idleThread;

public:
    Scheduler(const Scheduler& obj)=delete;
    static Scheduler* getScheduler();
    static PCB *get();
    static void put(PCB* p);
    void idle(void *arg);
    //static int clear(PCB* p);



};
#endif //PROJECT_BASE_V1_1_SCHEDULER_HPP
