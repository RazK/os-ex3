//
// Created by Shimon Heimowitz on 02/06/2018.
//

#ifndef CONTEXT_H
#define CONTEXT_H

#include "MapReduceClient.h"
#include "ErrorCodes.h"
#include "Barrier.h"
#include <algorithm>    // std::sort
#include <atomic>
#include <semaphore.h>
//#include <vector> //sdt:vec
//#include <utility> //std:pair


class Context{

public:
    Context(const MapReduceClient& client,
            const InputVec& inputVec, OutputVec& outputVec,
            int multiThreadLevel);
    ~Context();

    void append(tindex i, const IntermediatePair& pair);
    void sort(tindex i);

    const int numOfIntermediatesVecs;
    const MapReduceClient & client;



    const InputVec & inputVec;
    IntermediateVec * intermedVecs[];
    OutputVec & outputVec;

    Barrier barrier;
    pthread_mutex_t shuffleMutex;
    pthread_mutex_t outVecMutex;
    std::atomic<bool> shuffleLocked;
    std::atomic<unsigned long> counter;
    sem_t queueSem;
};



#endif //CONTEXT_H
