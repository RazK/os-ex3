//
// Created by Shimon Heimowitz on 02/06/2018.
//

#ifndef GIT_FRAMEWORK_H
#define GIT_FRAMEWORK_H

#include "MapReduceFramework.h"
#include "MapReduceClient.h"

#include "Context.h"
#include "ErrorCodes.h"
#include <pthread.h>

#include <vector> //sdt:vec
#include <utility> //std:pair


class FrameWork{

public:
    FrameWork(const MapReduceClient& client,
              const InputVec& inputVec, OutputVec& outputVec,
              int multiThreadLevel);
    ErrorCode run();
    ~FrameWork();



private:
    void * threadWork(void * arg);

//    std::vector<Context> threadContextVec;


    const MapReduceClient& client;
    const InputVec& inputVec;
    OutputVec& outputVec;
    int numOfThreads;
    Context* threadContextVec;

    std::atomic<unsigned long> atomic_counter;
//    std::vector<pthread_t> threadPool;
    pthread_t* threadPool;
    bool shuffleLocked;



};

#endif //GIT_FRAMEWORK_H
