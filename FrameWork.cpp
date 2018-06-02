//
// Created by Shimon Heimowitz on 02/06/2018.
//


#include "FrameWork.h"
#include <pthread.h>

ContextWrapper::ContextWrapper(int threadIndex, Context * context) {

    this->threadIndex = threadIndex;
    this->context = context;

}


void * threadWork(void * contextWrapper) {
    auto contextWrapperPtr  = static_cast<ContextWrapper*> (contextWrapper);
    int threadIndex = contextWrapperPtr->threadIndex;
    Context* context = contextWrapperPtr->context;

    //Hungry map loop
    unsigned long old_value = 0;
    while(old_value < context->inputVec.size())
    {
        old_value = context->counter++; //atomic
        contextWrapperPtr->context->client.map( context->inputVec.at(old_value).first,
                                                context->inputVec.at(old_value).second,
                                                contextWrapper);
//                   static_cast<void *>(&threadContextVec[*contextWrapperPtr->tindex]));
    }

    // intermediate vector assumed to be populated at this point

    // Sorting Stage - No mutually shared objects
    context->sort(threadIndex);

    // Barrier for all threads
    context->barrier.barrier();

    //After Barrier.
    //One thread becomes shuffler

//    context->shuffleLocked = true;
    if(pthread_mutex_lock(&context->shuffleMutex) != ErrorCode::SUCCESS) {
        printf("Error\n");
        exit(1);
    }

    if (context->shuffleLocked == false){
        // lock for the rest of the threads
        context->shuffleLocked = true;
        sem_wait(&context->queueSem);

        // and party
        //Todo: Shuffle phase - raz.. shine
        //Todo: Remember to send signal via semaphore. whenever the queue is read,
        // use sem_post(context->queueSem)




    }

}



FrameWork::FrameWork(const MapReduceClient &client, const InputVec &inputVec, OutputVec &outputVec,
                     int multiThreadLevel)
: client(client),
  numOfThreads(multiThreadLevel),
  atomic_counter(0),
  inputVec(inputVec),
  outputVec(outputVec),
  shuffleLocked(false),
  barrier(Barrier(multiThreadLevel)),
  threadPool(new pthread_t[multiThreadLevel]),
  context(client, inputVec, outputVec, multiThreadLevel)
{
    // init semaphore for ready queue sharing
    if (sem_init(&sortedQueueSem, 0, 0) != ErrorCode::SUCCESS)
    {
        printf("ERROR\n");
        exit(1);
    }

    //Todo: May nee to truncate the number of threads in use to the size of input vector.

//    this->barrier = Barrier(multiThreadLevel);
//    this->shuffleLocked = false;
//    this->threadPool = std::vector<pthread_t>();
}

ErrorCode FrameWork::run() {
    ErrorCode status[numOfThreads];

        //    Spawn threads on work function
    for (int t_index=0; t_index<this->numOfThreads; t_index++){
        if ((pthread_create(&threadPool[t_index], nullptr, threadWork, (void *)t_index)) !=
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
