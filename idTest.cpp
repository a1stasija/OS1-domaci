//
// Created by os on 9/20/23.
//
#include "../h/syscall_cpp.hpp"
#include "../test/printing.hpp"
typedef int Vector2D[2];

class Worker: public Thread {
    void worker();
public:
    Worker(const Vector2D* vect ,int cap,int num, int* res, Semaphore* m):Thread() {source=vect;N=cap;myJob=num;result=res;mutex=m;}
    int* s;
private:
    int myJob;
    int N;
    int* result;
    const Vector2D* source;
    Semaphore* mutex;
    void run() override {
        worker();
    }
};
void Worker::worker(){
    int sum=0;
    int i=0;
    while(i<N){
        sum+=source[myJob][i];
        i++;
    }
    //mutex->wait();
    *result+=sum;
   // mutex->signal();
    printInt(*result);
}



int idMain(void* p){
    Thread* threads[4];
    Vector2D mat[4]={
            {0,0},
            {1,1},
            {2,2},
            {3,3}
    };


    static int res=0;
    static Semaphore* mutex;
    mutex = new Semaphore(1);
    threads[0] = new Worker(mat, 2,0,&res,mutex);
    printString("ThreadA created\n");

    threads[1] = new Worker(mat, 2,1,&res,mutex);
    printString("ThreadB created\n");

    threads[2] = new Worker(mat, 2,2,&res,mutex);
    printString("ThreadC created\n");

    threads[3] = new Worker(mat, 2,3,&res,mutex);
    printString("ThreadD created\n");

    threads[0]->start();
    threads[1]->start();
    threads[2]->start();
    threads[3]->start();


    while (true) {
        Thread::dispatch();
    }

    for (auto thread: threads) { delete thread;
    }

}