//
// Created by razk on 02/06/18.
//
#include "Context.h"
#include "FrameWork.h"
#include <algorithm>    // std::sort

Context::Context(const MapReduceClient& client,
                 const InputVec& inputVec, OutputVec& outputVec,
                 int multiThreadLevel) :
        numOfIntermediatesVecs(multiThreadLevel),
        client(client),
        inputVec(inputVec),
        outputVec(outputVec),
        inputSize(inputVec.size()),
        uniqueK2Size(0),
        barrier(multiThreadLevel),
        shuffleState(ShuffleState::WAITING_FOR_SHUFFLER),
        mapTaskCounter(0),
        reduceTaskCounter(0),
        ac(0)

//        , queueSem(0)
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
//     already did in init list
    if (sem_init(&queueSem, 0, 0) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Failure to init semaphore in Context init.\n");
        exit(1);
    }

    this->intermedVecs = new IntermediateVec[numOfIntermediatesVecs];
    this->uniqueK2Vecs = new IntermediateUniqueKeysVec[numOfIntermediatesVecs];


    // Initialize empty intermediate pairs
//    for (int i = 0; i < numOfIntermediatesVecs; i++){
//        this->intermedVecs[i] = IntermediateVec();
//        this->uniqueK2Vecs[i] = IntermediateUniqueKeysVec();
//    }

}

Context::~Context() {
    // Delete all intermediate pairs
//    for (int i = 0; i < numOfIntermediatesVecs; i++){
    delete [] this->uniqueK2Vecs;
    delete [] this->intermedVecs;


    // Destroy shuffle mutex
    if (pthread_mutex_destroy(&shuffleMutex) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Failure destroy a mutex 1 (for the shuffling stage) in Context dtor.\n");
        exit(1);
    }
    if (pthread_mutex_destroy(&outVecMutex) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Failure destroy a mutex 2 (for the output vec) in Context dtor.\n");
        exit(1);
    }
    if (pthread_mutex_destroy(&queueMutex) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Failure destroy a mutex 3(for readyqueue) in Context dtor.\n");
        exit(1);
    }
    if (sem_destroy(&queueSem) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Failure destroy semaphore in Context dtor.\n");
        exit(1);
    }
//    queueSem.~Semaphore() ;
}


void Context::prepareForShuffle(const tindex i) {
    // Sort intermediate vecotr
    if(!intermedVecs[i].empty()){
        std::sort(this->intermedVecs[i].begin(), this->intermedVecs[i].end(), Pair2lessthan);

        // List all unique keys (will be used for shuffle)
        std::transform(intermedVecs[i].begin(), intermedVecs[i].end(), back_inserter(this->uniqueK2Vecs[i]), [](IntermediatePair& pair){return pair.first;});
        IntermediateUniqueKeysVec::iterator it;
        it = std::unique(this->uniqueK2Vecs[i].begin(), this->uniqueK2Vecs[i].end(), K2equals);
        this->uniqueK2Vecs[i].resize((unsigned long)std::distance(this->uniqueK2Vecs[i].begin(), it));
    }
}


void Context::append(const tindex i, const IntermediatePair& pair) {
    this->intermedVecs[i].push_back(pair);
}