//
// Created by Shimon Heimowitz on 02/06/2018.
//

#ifndef GIT_FRAMEWORK_H
#define GIT_FRAMEWORK_H

#include "MapReduceFramework.h"
#include "MapReduceClient.h"

#include "Context.h"
#include "ErrorCodes.h"
#include "Barrier.h"
#include <pthread.h>

#include <semaphore.h>
#include <atomic>
#include "Context.h"

#include <vector> //sdt:vec
#include <utility> //std:pair

struct ContextWrapper {
    ContextWrapper(int threadIndex, Context* c);
    Context* context;
    int threadIndex;
};

class FrameWork{

public:
    FrameWork(const MapReduceClient& client,
              const InputVec& inputVec, OutputVec& outputVec,
              int multiThreadLevel);
    ErrorCode run();
    ~FrameWork();



private:
    const MapReduceClient& client;
    int numOfThreads;
    std::atomic<unsigned long> atomic_counter;
    const InputVec& inputVec;
    OutputVec& outputVec;
    std::atomic<bool> shuffleLocked;
    Barrier barrier;
    sem_t sortedQueueSem;
    pthread_t* threadPool;
    Context context;

};

#endif //GIT_FRAMEWORK_H
