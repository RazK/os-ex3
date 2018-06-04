//
// Created by Shimon Heimowitz on 02/06/2018.
//

#ifndef CONTEXT_H
#define CONTEXT_H

#include "MapReduceClient.h"
#include "MapReduceFramework.h"
#include "ErrorCodes.h"
#include "Barrier.h"
#include "Semaphore.h"
#include <algorithm>    // std::sort
#include <atomic>
#include <semaphore.h>
#include <set>
#include <cstdio>
//#include <vector> //sdt:vec
//#include <utility> //std:pair

typedef int tindex;

typedef std::vector<K2*> IntermediateUniqueKeysVec;

typedef enum _ShuffleState{
    WAITING_FOR_SHUFFLER = 0,
    IN_SHUFFLE,
    DONE_SHUFFLING
} ShuffleState;

class Context{

public:
    Context(const MapReduceClient& client,
            const InputVec& inputVec, OutputVec& outputVec,
            int multiThreadLevel);
    ~Context();

    //  Methods
    void append(tindex i, const IntermediatePair& pair);
    void prepareForShuffle(tindex i);

    // Data members
    const int numOfIntermediatesVecs;

    const InputVec & inputVec;
    const MapReduceClient & client;
    OutputVec & outputVec;

    IntermediateVec * intermedVecs; // Map result vectors
    IntermediateUniqueKeysVec * uniqueK2Vecs; // Unique keys

    std::vector<IntermediateVec> readyQueue;

    const unsigned long inputSize;
    Barrier barrier;
    pthread_mutex_t shuffleMutex;
    pthread_mutex_t outVecMutex;
    pthread_mutex_t queueMutex;

    std::atomic<ShuffleState> shuffleState;
    std::atomic<unsigned long> counter;

    sem_t queueSem; //deprecated
//    Semaphore queueSem;

};



#endif //CONTEXT_H
