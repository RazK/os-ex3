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
//#include <vector> //sdt:vec
//#include <utility> //std:pair


class Context{

public:
    Context(const MapReduceClient& client,
            const InputVec& inputVec, OutputVec& outputVec,
            int multiThreadLevel);
    ~Context();

    void append(const IntermediatePair& pair);
    void sort(tindex i);

    const int numOfIntermediatesVecs;
    const MapReduceClient & client;

    const InputVec & inputVec;
    IntermediateVec * intermedVecs;
    OutputVec & outputVec;

    Barrier shuffleBarrier;
    pthread_mutex_t shuffleMutex;
    std::atomic<bool> shuffleLocked;
};



#endif //CONTEXT_H
