//
// Created by Shimon Heimowitz on 02/06/2018.
//

#include "MapReduceFramework.h"
#include "FrameWork.h"
#include "ErrorCodes.h"
#include "Context.h"

void emit2 (K2* key, V2* value, void* context){
    auto myContext = static_cast<Context*>(context);
    IntermediatePair pair2_ptr = IntermediatePair(key, value);
    myContext->append(pair2_ptr);
}

void emit3 (K3* key, V3* value, void* context){
    // All threads want to use the OutputVector
    // TODO: Solve using cv_wait to avoid busy_waiting
    if (pthread_mutex_lock(&mutex) != 0){
        fprintf(stderr, "[[Barrier]] error on pthread_mutex_lock");
        exit(1);
    }
    if (++count < numThreads) {
        if (pthread_cond_wait(&cv, &mutex) != 0){
            fprintf(stderr, "[[Barrier]] error on pthread_cond_wait");
            exit(1);
        }
    } else {
        count = 0;
        if (pthread_cond_broadcast(&cv) != 0) {
            fprintf(stderr, "[[Barrier]] error on pthread_cond_broadcast");
            exit(1);
        }
    }
    if (pthread_mutex_unlock(&mutex) != 0) {
        fprintf(stderr, "[[Barrier]] error on pthread_mutex_unlock");
        exit(1);
    }
}

void runMapReduceFramework(const MapReduceClient& client,
                           const InputVec& inputVec, OutputVec& outputVec,
                           int multiThreadLevel){
    FrameWork framework(client, inputVec, outputVec, multiThreadLevel);
    framework.run();


}

