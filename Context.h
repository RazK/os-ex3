//
// Created by Shimon Heimowitz on 02/06/2018.
//

#ifndef CONTEXT_H
#define CONTEXT_H

#include "MapReduceClient.h"
#include "ErrorCodes.h"
#include <algorithm>    // std::sort
//#include <vector> //sdt:vec
//#include <utility> //std:pair


class Context{

public:
    Context();
    ~Context();

    void append(const IntermediatePair& pair);

    void sort();

//private:
////    IntermediateVec* emit2Accumulator;
//    IntermediateVec emit2Accumulator;
//private:
    IntermediateVec* emit2Accumulator;
};



#endif //CONTEXT_H
