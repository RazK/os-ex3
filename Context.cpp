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
        shuffleLocked(false){
    // Initialize shuffle mutex
    pthread_mutex_init(&shuffleMutex, nullptr);

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
    pthread_mutex_destroy(&shuffleMutex);
}

void Context::sort(tindex i) {
    std::sort(this->intermedVecs[i]->begin(), this->intermedVecs[i]->end());
}


void Context::append(tindex i, const IntermediatePair& pair) {
    this->intermedVecs[i]->push_back(pair);
}