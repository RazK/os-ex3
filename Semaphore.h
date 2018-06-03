//
// Created by Shimon Heimowitz on 02/06/2018.
//

#ifndef OS_EX3_SEMAPHORE_H
#define OS_EX3_SEMAPHORE_H

#include "MapReduceClient.h"
#include "MapReduceFramework.h"
#include <atomic>
//#include <pthread>
#include "pthread.h"

class Semaphore{
public:
    explicit Semaphore(int n);
    ~Semaphore();

    void aquire();
    void release();

    void incSize();
    void decSize();

private:
    std::atomic<int> size;

    std::atomic<int> atomic_counter;

    pthread_mutex_t updateMutex;

};

#endif //OS_EX3_SEMAPHORE_H
