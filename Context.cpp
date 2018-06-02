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
        counter(0)
{
    // Initialize multiple mutexes
    if (pthread_mutex_init(&shuffleMutex, nullptr) != ErrorCode::SUCCESS){
        printf("ERROR\n");
        exit(1);
    }
    if (pthread_mutex_init(&outVecMutex, nullptr) != ErrorCode::SUCCESS){
        printf("ERROR\n");
        exit(1);
    }
    if (pthread_mutex_init(&queueMutex, nullptr) != ErrorCode::SUCCESS){
        printf("ERROR\n");
        exit(1);
    }

    // init semaphore
    if (sem_init(sem=&queueSem, pshared=0, value=0) != ErrorCode::SUCCESS){
        printf("ERROR\n");
        exit(1);
    }

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
        printf("ERROR\n");
        exit(1);
    }
    if (pthread_mutex_destroy(&outVecMutex) != ErrorCode::SUCCESS){
        printf("ERROR\n");
        exit(1);
    }
    if (pthread_mutex_destroy(&queueMutex) != ErrorCode::SUCCESS){
        printf("ERROR\n");
        exit(1);
    }
    if (sem_destroy(&queueSem) != ErrorCode::SUCCESS){
        printf("ERROR\n");
        exit(1);
    }

}

void Context::sort(const tindex i) {
    std::sort(this->intermedVecs[i]->begin(), this->intermedVecs[i]->end());
}


void Context::append(const tindex i, const IntermediatePair& pair) {
    this->intermedVecs[i]->push_back(pair);
}