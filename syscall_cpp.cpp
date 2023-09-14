//
// Created by os on 8/2/23.
//
#include "../h/syscall_cpp.hpp"


Thread::Thread(void (*body)(void *), void *arg) {
    this->body=body;
    this->arg=arg;
}
Thread::Thread() {
   body= nullptr;
}
Thread::~Thread() {
    delete myHandle;
}

int Thread::start() {
    if(!body) {
        thread_create(&myHandle, &wrapper, this);
        return 0;
    }
    else{
        thread_create(&myHandle,body,this->arg);
    }
    return 0;
}
void Thread::wrapper(void *arg) {
    Thread* t = (Thread*)arg;
    t->run();
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t time) {
    return 0;
}
void Thread::join() {
    thread_join(myHandle);
}

//--------------------------------------------------------------------------//

Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}




char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}

void *operator new(size_t size) {
    return mem_alloc(size);

}

void operator delete(void *p) {
    mem_free(p);

}