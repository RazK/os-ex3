//
// Created by Shimon Heimowitz on 02/06/2018.
//

#include "Semaphore.h"
#include "ErrorCodes.h"

Semaphore::Semaphore(int n) : size(n)
{
    this->atomic_counter.store(0);

}

void Semaphore::aquire() {
    while (true) {
        while (this->atomic_counter >= size) {}
        if (pthread_mutex_lock(&updateMutex) != ErrorCode::SUCCESS){
            fprintf(stderr, "Error: Could not lock mutex in semaphore aquire.");
            exit(1);
        }
        if (this->atomic_counter >= size) {
            if (pthread_mutex_unlock(&updateMutex) != ErrorCode::SUCCESS){
                fprintf(stderr, "Error: Could not unlock mutex in semaphore aquire.");
                exit(1);
            }
            continue;
        }
        ++this->atomic_counter;
        if (pthread_mutex_unlock(&updateMutex) != ErrorCode::SUCCESS){
            fprintf(stderr, "Error: Could not unlock mutex in semaphore aquire.");
            exit(1);
        }
        break;
    }

}

void Semaphore::release() {
    if (pthread_mutex_lock(&updateMutex) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Could not lock mutex in semaphore release.");
        exit(1);
    }
    if (this->atomic_counter > 0) {
        --this->atomic_counter;
    } else{
        fprintf(stderr, "Err: Semaphore found negative. Run in debug.");
    }
    if (pthread_mutex_unlock(&updateMutex) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Could not lock mutex in semaphore release.");
        exit(1);
    }

}

void Semaphore::incSize(){
    if (pthread_mutex_lock(&updateMutex) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Could not lock mutex in semaphore release.");
        exit(1);
    }
    if (this->size >= 0) {
        ++this->size;
    } else{
        fprintf(stderr, "Err: Semaphore found negative size. Run in debug.");
    }
    if (pthread_mutex_unlock(&updateMutex) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Could not lock mutex in semaphore release.");
        exit(1);
    }

}

Semaphore::~Semaphore() {
    if (pthread_mutex_destroy(&updateMutex) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Could not destroy mutex in semaphore dtor.");
        exit(1);
    }
//    delete(atomic_counter);

}

void Semaphore::decSize(){
    if (pthread_mutex_lock(&updateMutex) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Could not lock mutex in semaphore release.");
        exit(1);
    }
    if (this->size >= 0) {
        --this->size;
    } else{
        fprintf(stderr, "Err: Semaphore found negative size. Run in debug.");
    }
    if (pthread_mutex_unlock(&updateMutex) != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Could not lock mutex in semaphore release.");
        exit(1);
    }

}
