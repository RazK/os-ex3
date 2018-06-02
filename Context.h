//
// Created by Shimon Heimowitz on 02/06/2018.
//

#ifndef CONTEXT_H
#define CONTEXT_H

#include "MapReduceClient.h"
#include "ErrorCodes.h"
//#include <vector> //sdt:vec
//#include <utility> //std:pair


class Context{

public:
    Context();
    ~Context();

    int getTid();

    ErrorCode append(IntermediatePair);

private:
    IntermediateVec emit2Accumulator;
    int tid;

};



#endif //CONTEXT_H
