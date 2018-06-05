//
// Created by Shimon Heimowitz on 02/06/2018.
//


#include "FrameWork.h"
#include <cstdio>
#include <pthread.h>
#include <f2c.h>

#define FIRST_NONMAIN_THREAD_INDEX 1


ContextWrapper::ContextWrapper() {}

ContextWrapper::ContextWrapper(int threadIndex, Context * context) {

    this->threadIndex = threadIndex;
    this->context = context;

}

bool K2equals(const K2 *key1, const K2 *key2){
    return !((*key1 < *key2) || (*key2 < *key1));
}

bool K2lessthan(const K2 *key1, const K2 *key2){
    return *key1 < *key2;
}

bool Pair2lessthan(const IntermediatePair &p1, const IntermediatePair &p2){
    return *p1.first < *p2.first;
}

void * threadWork(void * contextWrapper) {
    // Unpack contextWrapper
    auto contextWrapperPtr = static_cast<ContextWrapper*> (contextWrapper);
    int threadIndex = contextWrapperPtr->threadIndex;
    Context* context = contextWrapperPtr->context;

    /************************************************
     *                  MAP PHASE                   *
     ************************************************/

    //Hungry map loop
    unsigned long old_value = 0;
    while((old_value = context->mapTaskCounter++) < context->inputSize) {
        context->client.map( context->inputVec[old_value].first,
                             context->inputVec[old_value].second,
                             contextWrapper);
    }

    // My intermediate vector assumed to be populated at this point
    // Sorting Stage - No mutually shared objects
    context->prepareForShuffle(threadIndex);

    /************************************************
     *                  BARRIER                     *
     ************************************************/

    // Barrier for all threads
    context->barrier.barrier(threadIndex);

    /************************************************
     *              SHUFFLE \ REDUCE                *
     ************************************************/

    /************************************************
     *                  SHUFFLE                     *
     ************************************************/
    // first to retrive 0 (atomically) is crowned shuffler. Long shall he reign!
    int first = context->shufflerRace++;
    if (first == 0){

        // lock for the rest of the threads
        context->shuffleState = ShuffleState::IN_SHUFFLE;

        // Collect all unique keys from all intermediate unique keys vectors
        IntermediateUniqueKeysVec uniKeys; // Example: uniqueK2Vecs = {[1,2,3], [2,3], [1,3]}
        for (int i = 0; i < context->numOfIntermediatesVecs; i++) {
            std::copy(context->uniqueK2Vecs[i].begin(), context->uniqueK2Vecs[i].end(), back_inserter(uniKeys));   // 10 20 30 20 10 0  0  0  0
        }
        // Unify into single vector of ordered unique keys
        std::sort(uniKeys.begin(), uniKeys.end(), K2lessthan);
        IntermediateUniqueKeysVec::iterator it;
        it = std::unique(uniKeys.begin(), uniKeys.end(), K2equals);   // 10 20 30 20 10 ?  ?  ?  ?
        uniKeys.resize((unsigned long)std::distance(uniKeys.begin(), it) ); // 10 20 30 20 10
        context->uniqueK2Size = uniKeys.size();

         /************************************************
         *                  PRODUCE TASKS                *
         ************************************************/
        // Go over ordered unique keys, foreach pop all pairs with this key from all vectors and launch reducer
        while(!uniKeys.empty()){
            // Get current key and extract all its pairs from all vectors
            K2* currKey = uniKeys.back();
            uniKeys.pop_back();
            auto keySpecificVec = IntermediateVec(); // TODO: Free at the end of reducer's procedure
            // Go over all intermediate vectors
            for (int j = 0; j < context->numOfIntermediatesVecs; j++) {
                // Extract all pairs with current key (if has any)
                while ((!context->intermedVecs[j].empty()) &&
                        K2equals(context->intermedVecs[j].back().first, currKey)){
                    keySpecificVec.push_back(context->intermedVecs[j].back());
                    context->intermedVecs[j].pop_back();
                }
            } // All pairs with current key were processed into keySpecificVec - ready to reduce!

            if (sem_wait(&context->taskQueueSem) != ErrorCode::SUCCESS)
            {
                fprintf(stderr, "Error: SHUFFLER Mutex (queue mutex) lock failure in waiting thread.\n");
                exit(1);
            }

            context->readyQueue.push_back(keySpecificVec);

            if (sem_post(&context->taskQueueSem) != ErrorCode::SUCCESS)
            {
                fprintf(stderr, "Error: SHUFFLER Mutex (queue mutex) UNlock failure in waiting thread.\n");
                exit(1);
            }

            if (sem_post(&context->taskCountSem) != ErrorCode::SUCCESS)
            {
                fprintf(stderr, "Error: SHUFFLER Failed to post semaphore in shuffle stage.\n");
                exit(1);
            }
        }
        context->shuffleState = ShuffleState::DONE_SHUFFLING;

        //special magic loop to release all remaining threads stuck on sem for task counter:
        for (int i=0; i<context->numOfIntermediatesVecs; i++){

            if (sem_post(&context->taskCountSem) != ErrorCode::SUCCESS)
            {
                fprintf(stderr, "Error: SHUFFLER Failed to post semaphore in release loop\n");
                exit(1);
            }
            if (sem_post(&context->taskQueueSem) != ErrorCode::SUCCESS)
            {
                fprintf(stderr, "Error: SHUFFLER Failed to post semaphore in release loop\n");
                exit(1);
            }

        }
    }

    /************************************************
     *                  REDUCE                      *
     ************************************************/

    // All threads continue here. ShuffleLocked represents the shuffler is still working
    unsigned long task_num = 0;
    while(true){
        if (context->reduceTaskCounter >= context->uniqueK2Size){
            break;
        }

        // Wait for the shuffler to populate queue. Signal comes through semaphore
        if (sem_wait(&context->taskCountSem) != ErrorCode::SUCCESS)
        {
            fprintf(stderr, "Error: REDUCER Semaphore failure in waiting thread.\n");
            exit(1);
        }

        // Lock the mutex to access mutual queue
        if (sem_wait(&context->taskQueueSem) != ErrorCode::SUCCESS)
        {
            fprintf(stderr, "Error: REDUCER Mutex lock failure in waiting thread.\n");
            exit(1);
        }

        // Leave if all tasks were performed
        task_num = context->reduceTaskCounter++;

        IntermediateVec job;
        // retrieve next job if exists
        if (task_num < context->uniqueK2Size){
            job = context->readyQueue[task_num];

        }

        if (sem_post(&context->taskQueueSem) != ErrorCode::SUCCESS)
        {
            fprintf(stderr, "Error: REDUCER Mutex unlock failure in waiting thread.\n");
            exit(1);
        }

        if (task_num < context->uniqueK2Size){
            context->client.reduce(&job, contextWrapper);

        }
    }

    return (void *)ErrorCode::SUCCESS;
}


FrameWork::FrameWork(const MapReduceClient &client, const InputVec &inputVec, OutputVec &outputVec,
                     int multiThreadLevel) :
  numOfThreads(multiThreadLevel),
  threadPool(new pthread_t[multiThreadLevel]),
  context(client, inputVec, outputVec, multiThreadLevel)
{
    if (multiThreadLevel < 1){
        fprintf(stderr, "Error: multithread level %d is illegal.\n", multiThreadLevel);
        exit(-1);
    }
}

ErrorCode FrameWork::run() {
    //    Spawn threads on work function
    ContextWrapper context_vec[numOfThreads];
    for (int i = 0; i < numOfThreads; i++) {
        context_vec[i] = ContextWrapper(i, &this->context);
    }

    // Runs on all none main threads. First thread is considered thread 0
    for (int t_index = FIRST_NONMAIN_THREAD_INDEX; t_index < this->numOfThreads; t_index++){
        if (ErrorCode::SUCCESS != pthread_create(&threadPool[t_index],
                                                 nullptr,
                                                 threadWork,
                                                 static_cast<void *>(&context_vec[t_index])))
        {
            fprintf(stderr, "Error: Failure to spawn new thread in run.\n");
            exit(-1);
        }
    }

//     Run main thread's task
    auto curStatus = threadWork(static_cast<void *>(&context_vec[0]));
    if (ErrorCode::SUCCESS != *(ErrorCode*)&curStatus){
        fprintf(stderr, "Error: threads with t_index 0 did not succeed in threadWork.\n");
        exit(-1);
    }

    // Join all none main threads back into 1
    for (int t_index = FIRST_NONMAIN_THREAD_INDEX; t_index<this->numOfThreads; t_index++){
        ErrorCode curStatus = ErrorCode::UNINITIALIZED;
        if (ErrorCode::SUCCESS != pthread_join(threadPool[t_index], (void**)&curStatus)) {
            fprintf(stderr, "Error: Failure to join threads in run.\n");
            exit(-1);
        }
        if (ErrorCode::SUCCESS != curStatus){
            fprintf(stderr, "Error: threads with t_index %d did not succeed in threadWork.\n", t_index);
            exit(-1);
        }
    }
    return SUCCESS;
}

FrameWork::~FrameWork() {
    delete [] threadPool;
}
