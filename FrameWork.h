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

bool K2equals(const K2 *key1, const K2 *key2);
bool Pair2lessthan(const IntermediatePair &p1, const IntermediatePair &p2);

class FrameWork{

public:
    FrameWork(const MapReduceClient& client,
              const InputVec& inputVec, OutputVec& outputVec,
              int multiThreadLevel);
    ErrorCode run();
    ~FrameWork();



private:
    int numOfThreads;
    pthread_t* threadPool;
    Context context;

};

#endif //GIT_FRAMEWORK_H
