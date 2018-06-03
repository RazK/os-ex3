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
    while(context->counter < context->inputVec.size())
    {
        old_value = context->counter++; //atomic
        contextWrapperPtr->context->client.map( context->inputVec.at(old_value).first,
                                                context->inputVec.at(old_value).second,
                                                contextWrapper);
    }

    // intermediate vector assumed to be populated at this point

    // Sorting Stage - No mutually shared objects
    context->sort(threadIndex);

    // Barrier for all threads
    context->barrier.barrier();

    //After Barrier.One thread becomes shuffler
    if(pthread_mutex_lock(&context->shuffleMutex) != ErrorCode::SUCCESS) {
        fprintf(stderr, "Error: Mutex lock failure in shuffle thread, after barrier.\n");
        exit(1);
    }

    if (context->shuffleLocked == false){
        // lock for the rest of the threads
        context->shuffleLocked = true;

        // Let the rest of the threads run
        if(pthread_mutex_unlock(&context->shuffleMutex) != ErrorCode::SUCCESS) {
            fprintf(stderr, "Error: Mutex unlock failure in shuffle thread, after barrier.\n");
            exit(1);
        }
        for (int tid=0; tid<context->numOfIntermediatesVecs; tid++){

        // Find all unique keys in all intermediate vectors
        IntermediateKeySet uniKeys;
        for (int i = 0; i < context->numOfIntermediatesVecs; i++) {
            for (const auto &key : *context->uniqueK2set[i]) {
                uniKeys.insert(key);
            }
        }

        // Todo: use context->queueSem.incSize();

        // This notifies all threads shuffle stage is over.
        context->shuffleLocked = false;
    }
    // All threads continue here. ShuffleLocked represents the shuffler is still working
    while(context->shuffleLocked || not context->readyQueue.empty()){
        // Wait for the shuffler to populate queue. Signal comes through semaphore
//        if (sem_wait(&context->queueSem) != ErrorCode::SUCCESS)
//        {
//            fprintf(stderr, "Error: Semaphore failure in waiting thread.\n");
//            exit(1);
//        }
        context->queueSem.aquire();
        context->queueSem.decSize();

        // Lock the mutex to access mutual queue
        if (pthread_mutex_lock(&context->queueMutex) != ErrorCode::SUCCESS)
        {
            fprintf(stderr, "Error: Mutex lock failure in waiting thread.\n");
            exit(1);
        }
        // retrieve next job and pop it from the list
        IntermediateVec job = context->readyQueue.back();
        context->readyQueue.pop_back();
        if (pthread_mutex_unlock(&context->queueMutex) != ErrorCode::SUCCESS)
        {
            fprintf(stderr, "Error: Mutex unlock failure in waiting thread.\n");
            exit(1);
        }
        context->client.reduce(&job, contextWrapper);


    }
    return (void *)ErrorCode::SUCCESS;
}



FrameWork::FrameWork(const MapReduceClient &client, const InputVec &inputVec, OutputVec &outputVec,
                     int multiThreadLevel) :
// client(client),
  numOfThreads(multiThreadLevel),
//  atomic_counter(0),
//  inputVec(inputVec),
//  outputVec(outputVec),
//  shuffleLocked(false),
//  barrier(Barrier(multiThreadLevel)),
  threadPool(new pthread_t[
          multiThreadLevel]),
  context(client, inputVec, outputVec, multiThreadLevel)
{


    //Todo: May nee to truncate the number of threads in use to the size of input vector.

}

ErrorCode FrameWork::run() {
    ErrorCode status[numOfThreads];

    //    Spawn threads on work function
    for (int t_index=0; t_index<this->numOfThreads; t_index++){
        if ((pthread_create(&threadPool[t_index], nullptr, threadWork,
                            static_cast<void *>(&context))) != ErrorCode::SUCCESS) {
            fprintf(stderr, "Error: Failure to spawn new thread in run.\n");
            exit(-1);
        }
    }

    //    Join all threads back into 1
    for (int t_index=0; t_index<this->numOfThreads; t_index++){
        if (pthread_join(threadPool[t_index], (void **)&status[t_index]) != ErrorCode::SUCCESS) {
            fprintf(stderr, "Error: Failure to join threads in run.\n");
            exit(-1);
        }
    }

    return SUCCESS;
}

FrameWork::~FrameWork() {
//    return;
}
