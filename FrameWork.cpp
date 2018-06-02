//
// Created by Shimon Heimowitz on 02/06/2018.
//


#include "FrameWork.h"
#include "ErrorCodes.h"
#include <atomic>

FrameWork::FrameWork(const MapReduceClient &client, const InputVec &inputVec, OutputVec &outputVec,
                     int multiThreadLevel)
: client(client),
  inputVec(inputVec),
  outputVec(outputVec),
  numOfThreads(multiThreadLevel),
  threadContextVec(new Context[multiThreadLevel]),
  atomic_counter(0),
  threadPool(new pthread_t[multiThreadLevel])
{
//    this->threadPool = std::vector<pthread_t>();
}

ErrorCode FrameWork::run() {
    ErrorCode status[numOfThreads];

        //    Spawn threads on work function
    for (int t_index=0; t_index<this->numOfThreads; t_index++){
        if ((pthread_create(&threadPool[t_index], NULL, FrameWork::threadWork, (void *)t_index)) !=
            ErrorCode::SUCCESS)
        {
            printf("ERROR\n");
            exit(-1); }
        }

        //    Join all threads back into 1
    for (int t_index=0; t_index<this->numOfThreads; t_index++){
        if (pthread_join(threadPool[t_index], (void **)&status[t_index]) != ErrorCode::SUCCESS)
        {
            printf("ERROR\n");
            exit(-1); }
        }

    return SUCCESS;
}

FrameWork::~FrameWork() {
//    return;
}

void* FrameWork::threadWork(void * arg) {
    int t_index  = static_cast<int> (arg);

    //Hungry map loop
    unsigned long old_value = 0;
    while(old_value < this->inputVec.size())
    {
        old_value = atomic_counter++;
        client.map(inputVec.at(old_value).first,
                   inputVec.at(old_value).second,
                   static_cast<void *>(&threadContextVec[t_index]));
    }

    // intermediate vector assumed to be populated at this point

    // Sorting Stage - No mutually shared objects



    return static_cast<void *>(ErrorCode::FAIL);
}
