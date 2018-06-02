//
// Created by Shimon Heimowitz on 02/06/2018.
//


#include "FrameWork.h"
#include "ErrorCodes.h"

FrameWork::FrameWork(const MapReduceClient &client, const InputVec &inputVec, OutputVec &outputVec,
                     int multiThreadLevel)
: client(client), inputVec(inputVec), outputVec(outputVec), numOfThreads(multiThreadLevel),
  threadContextVec(new std::vector<Context>)
{
//    return ;
}

ErrorCode FrameWork::run() {
    return ErrorCode();
}

FrameWork::~FrameWork() {
//    return;
}

ErrorCode FrameWork::threadWork() {


    return ErrorCode::FAIL;
}
