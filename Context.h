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

    void append(const IntermediatePair& pair);

//private:
    IntermediateVec* emit2Accumulator;
};



#endif //CONTEXT_H
