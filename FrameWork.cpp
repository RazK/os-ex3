//
// Created by Shimon Heimowitz on 02/06/2018.
//


#include "FrameWork.h"

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
    if(pthread_mutex_lock(context->shufflemutex) != ErrorCode::SUCCESS) {
        printf("Error\n");
        exit(1);
    }

    if (context->shuffleLocked == false){
        // lock for the rest of the threads
        context->shuffleLocked = true;
        sem_wait(context->queueSem);

        // and party
        //Todo: Shuffle phase - raz.. shine
        //Todo: Remember to send signal via semaphore. whenever the queue is read,
        // use sem_post(context->queueSem)




    }
    while


}



FrameWork::FrameWork(const MapReduceClient &client, const InputVec &inputVec, OutputVec &outputVec,
                     int multiThreadLevel)
: client(client),
  inputVec(inputVec),
  outputVec(outputVec),
  numOfThreads(multiThreadLevel),
  threadContextVec(new Context[multiThreadLevel]),
  atomic_counter(0),
  shuffleLocked(false),
  threadPool(new pthread_t[multiThreadLevel]),
  barrier(Barrier(multiThreadLevel))
//  sortedQueueSem()

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



//
//void* FrameWork::threadWork(void * arg) {
//    int* t_index_ptr  = static_cast<int*> (arg);
//
//    //Hungry map loop
//    unsigned long old_value = 0;
//    while(old_value < this->inputVec.size())
//    {
//        old_value = atomic_counter++;
//        client.map(inputVec.at(old_value).first,
//                   inputVec.at(old_value).second,
//                   static_cast<void *>(&threadContextVec[*t_index_ptr]));
//    }
//
//    // intermediate vector assumed to be populated at this point
//
//    // Sorting Stage - No mutually shared objects
//    threadContextVec[*t_index_ptr].sort();
//
//    // Barrier for all threads
//    this->barrier.barrier();
//
//    //After Barrier.
//    //One thread becomes shuffler
//
//    if (shuffleLocked == false){
//        // lock for the rest of the threads
//        shuffleLocked = true;
//
//        // and party
//        //Todo: Shuffle phase - raz.. shine
//        //Todo: Remember to send
//
//    }







//    return static_cast<void *>(ErrorCode::FAIL);
    return NULL;
}


