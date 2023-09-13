#include "../h/KSemaphore.hpp"

KSemaphore::KSemaphore(int val){
    head=tail= nullptr;
    value=val;
}

KSemaphore ::~KSemaphore (){
    while(head){
        PCB* pom=head;
        pom->error=true;
        head=head->nextPCB;
        pom->nextPCB= nullptr;
        Scheduler::put(pom);
    }
    tail= nullptr;
}

void KSemaphore::wait() {
    value=value-1;
    if(value<0){
        if(head== nullptr){
            head=tail=PCB::running;
        }else{
            tail->nextPCB=PCB::running;
            tail=tail->nextPCB;
        }
        PCB::running->nextPCB= nullptr;
        PCB* old=PCB::running;
        PCB::running=Scheduler::get();
        PCB::contextSwitch(&old->context,&PCB::running->context);

    }
}

int KSemaphore::signal() {
    value=value+1;
    if(value<=0){
        if(!head){
            return -1; //neregularno (niko ne ceka na semaforu a on ima vrednost manju od 0)
        }
        PCB* pom=head;
        head=head->nextPCB;
        pom->nextPCB= nullptr;
        if(!pom->isFinished()){
            Scheduler::put(pom);
        }else{
            return -2;  //neregularno, vracamo mrtav proces u spremne
        }
    }
    return 0;
}

KSemaphore* KSemaphore::createKSemaphore(int v) {
    KSemaphore* s=new KSemaphore(v);
    if(s->value<0){
        return nullptr;
    }
    return s;
}

void *KSemaphore::operator new(size_t size) {
    MemoryAllocator* mem=MemoryAllocator::getMemAlloc();
    return mem->kmem_alloc(size);


}
void KSemaphore::operator delete(void *p) {
    MemoryAllocator* mem=MemoryAllocator::getMemAlloc();
    mem->kmem_free(p);
}
void *KSemaphore::operator new[](size_t size) {
    MemoryAllocator* mem=MemoryAllocator::getMemAlloc();
    return mem->kmem_alloc(size);
}


void KSemaphore::operator delete[](void *p) {
    MemoryAllocator* mem=MemoryAllocator::getMemAlloc();
    mem->kmem_free(p);
}
