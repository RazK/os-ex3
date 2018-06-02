//
// Created by razk on 02/06/18.
//
#include "Context.h"
#include <algorithm>    // std::sort

Context::Context(const MapReduceClient& client,
                 const InputVec& inputVec, OutputVec& outputVec,
                 int multiThreadLevel) :
        client(client),
        numOfIntermediatesVecs(multiThreadLevel),
        inputVec(inputVec),
        outputVec(outputVec),
        barrier(multiThreadLevel),
        shuffleLocked(false),
        counter(0),
        queueSem(0)
{
    // Initialize multiple mutexes
    if (pthread_mutex_init(&shuffleMutex, nullptr) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Failure to init mutex in Context init.\n");
        exit(1);
    }
    if (pthread_mutex_init(&outVecMutex, nullptr) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Failure to init mutex in Context init.\n");
        exit(1);
    }
    if (pthread_mutex_init(&queueMutex, nullptr) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Failure to init mutex in Context init.\n");
        exit(1);
    }

    // init semaphore
    // already did in init list
//    if (sem_init(&queueSem, 0, 0) != ErrorCode::SUCCESS){
//        fprintf(stderr, "Error: Failure to init semaphore in Context init.\n");
//        exit(1);
//    }


    // Initialize empty intermediate pairs
    for (int i = 0; i < numOfIntermediatesVecs; i++){
        this->intermedVecs[i] = new std::vector<IntermediatePair>();
    }

}

Context::~Context() {
    // Delete all intermediate pairs
    for (int i = 0; i < numOfIntermediatesVecs; i++){
        delete this->intermedVecs[i];
    }

    // Destroy shuffle mutex
    if (pthread_mutex_destroy(&shuffleMutex) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Failure destroy a mutex in Context dtor.\n");
        exit(1);
    }
    if (pthread_mutex_destroy(&outVecMutex) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Failure destroy a mutex in Context dtor.\n");
        exit(1);
    }
    if (pthread_mutex_destroy(&queueMutex) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Failure destroy a mutex in Context dtor.\n");
        exit(1);
    }
//    if (sem_destroy(&queueSem) != ErrorCode::SUCCESS){
//        fprintf(stderr, "Error: Failure destroy semaphore in Context dtor.\n");
//        exit(1);
//    }
    queueSem.~Semaphore() ;

}

void Context::sort(const tindex i) {
    std::sort(this->intermedVecs[i]->begin(), this->intermedVecs[i]->end());
}


void Context::append(const tindex i, const IntermediatePair& pair) {
    this->intermedVecs[i]->push_back(pair);
}