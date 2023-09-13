//
// Created by os on 7/10/23.
//

#ifndef PROJECT_BASE_V1_1_SYSCALL_C_H
#define PROJECT_BASE_V1_1_SYSCALL_C_H

#include "../lib/hw.h"
#include "../h/PCB.hpp"
#include "../h/KSemaphore.hpp"

typedef struct PCB PCB;
typedef struct KSemaphore KSemaphore;

void* mem_alloc(size_t size);

int mem_free(void*);

typedef PCB* thread_t;

int thread_create(thread_t* handle, void (*start_routine)(void*),void* arg);

int thread_exit();

void thread_dispatch();

void thread_join(thread_t handle);


typedef KSemaphore* sem_t;
int sem_open(sem_t* handle, unsigned init);

int sem_close(sem_t handle);

int sem_wait(sem_t id);

int sem_signal(sem_t id);

typedef unsigned long time_t;
int time_sleep(time_t);

const int EOF=-1;
char getc();

void putc(char);


#endif //PROJECT_BASE_V1_1_SYSCALL_C_H
