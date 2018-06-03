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
    auto contextWrapperPtr = static_cast<ContextWrapper*> (contextWrapper);
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
    context->prepareForShuffle(threadIndex);

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
//        sem_wait(&context->queueSem); // TODO: Commented out for compilation, resolve this
        // Let the rest of the threads run
        if(pthread_mutex_unlock(&context->shuffleMutex) != ErrorCode::SUCCESS) {
            fprintf(stderr, "Error: Mutex unlock failure in shuffle thread, after barrier.\n");
            exit(1);
        }

        // Collect all unique keys from all intermediate unique keys vectors
        IntermediateUniqueKeysVec uniKeys;
        for (int i = 0; i < context->numOfIntermediatesVecs; i++) {
            std::copy(context->uniqueK2Vecs[i]->begin(), context->uniqueK2Vecs[i]->end(), back_inserter(uniKeys));   // 10 20 30 20 10 0  0  0  0
        }
        // Unify into single vector of ordered unique keys
        std::sort(uniKeys.begin(), uniKeys.end());
        IntermediateUniqueKeysVec::iterator it;
        it = std::unique (uniKeys.begin(), uniKeys.end());   // 10 20 30 20 10 ?  ?  ?  ?
        uniKeys.resize((long)std::distance(uniKeys.begin(), it) ); // 10 20 30 20 10

        // Go over ordered unique keys, foreach pop all pairs with this key from all vectors and launch reducer
        // DEBUG: Assuming keys in uniKeys are ordered just like in intermedVecs, i.e. uniKeys.back() is last in intermedVecs as well
        while(!uniKeys.empty()){
            // Get current key and extract all its pairs from all vectors
            K2* currKey = uniKeys.back();
            uniKeys.pop_back();
            auto * keySpecificVec = new IntermediateVec(); // TODO: Free at the end of reducer's procedure
            // Go over all intermediate vectors
            for (int j = 0; j < context->numOfIntermediatesVecs; j++) {
                // Extract all pairs with current key (if has any)
                while ((!context->intermedVecs[j]->empty()) && context->intermedVecs[j]->back().first == currKey){
                    keySpecificVec->push_back(context->intermedVecs[j]->back());
                    context->intermedVecs[j]->pop_back();
                }
            }
            // All pairs with current key were processed into keySpecificVec - ready to reduce!
            // LAUNCH REDUCER ON CURRENT KEY-SPECIFIC-VECTOR
            context->readyQueue.push_back(*keySpecificVec);
            // and party
            //Todo: Remember to send signal via semaphore. whenever the queue is read,
            // use sem_post(context->queueSem)
            // TODO: Shimmy: Implement your semaphore signal HERE

        }
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
