//
// Created by Shimon Heimowitz on 02/06/2018.
//

#ifndef CONTEXT_H
#define CONTEXT_H

#include <atomic>
#include "MapReduceClient.h"
#include "ErrorCodes.h"
#include "Barrier.h"
//#include <vector> //sdt:vec
//#include <utility> //std:pair


class Context{

public:
    Context(std::atomic<bool> const & suffleLocked, Barrier const & shuffleBarrier);
    ~Context();

    void append(const IntermediatePair& pair);

//private:
    IntermediateVec* emit2Accumulator;
    std::atomic<bool> const & shuffleLocked;
    Barrier const & shuffleBarrier;
};



#endif //CONTEXT_H
