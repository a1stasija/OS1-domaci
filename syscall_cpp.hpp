//
// Created by os on 7/10/23.
//

#ifndef PROJECT_BASE_V1_1_SYSCALL_CPP_HPP
#define PROJECT_BASE_V1_1_SYSCALL_CPP_HPP


#include "../h/syscall_c.h"

void* operator new(size_t);
void operator delete(void*);

class Thread{
public:
    Thread(void (*body)(void*),void* arg);
    virtual ~Thread();

    int start();

    void join();

    static void dispatch();
    static int sleep (time_t);

protected:
    Thread();
    virtual void run() {}

private:
    static void wrapper(void* arg);
    thread_t myHandle;
    void (*body)(void*);
    void* arg;
};

class Semaphore{
public:
    Semaphore(unsigned init=1);
    virtual ~Semaphore();

    int wait();
    int signal();

private:
    sem_t myHandle;
};
class Console{
public:
    static char getc();
    static void putc (char);
};

#endif //PROJECT_BASE_V1_1_SYSCALL_CPP_HPP
