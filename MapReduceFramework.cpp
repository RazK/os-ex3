//
// Created by Shimon Heimowitz on 02/06/2018.
//

#include "MapReduceFramework.h"
#include "FrameWork.h"
#include "ErrorCodes.h"
#include "Context.h"

void emit2 (K2* key, V2* value, void* context){
    auto contextWrap = static_cast<ContextWrapper*>(context);
    auto index = contextWrap->threadIndex;
    IntermediatePair pair = IntermediatePair(key, value);
    contextWrap->context->append(index, pair);
}

void emit3 (K3* key, V3* value, void* context){
    auto contextWrap = static_cast<ContextWrapper*>(context);
    auto threadContext = contextWrap->context;

    OutputPair pair = OutputPair(key, value);

    // All threads want to use the OutputVector
    // Lock the mutex for access
    if (pthread_mutex_lock(&threadContext->outVecMutex) != 0){
        fprintf(stderr, "Error: Failure to lock the mutex in emit3.\n");
        exit(1);
    }
    threadContext->outputVec.push_back(pair);

    if (pthread_mutex_unlock(&threadContext->outVecMutex) != 0){
        fprintf(stderr, "Error: Failure to unlock the mutex in emit3.\n");
        exit(1);
    }


    // TODO: Solve using cv_wait to avoid busy_waiting

}

void runMapReduceFramework(const MapReduceClient& client,
                           const InputVec& inputVec, OutputVec& outputVec,
                           int multiThreadLevel){
    FrameWork framework(client, inputVec, outputVec, multiThreadLevel);
    if (framework.run() != ErrorCode::SUCCESS){
        fprintf(stderr, "Error: Failure in run function. Run in debug.\n");
        exit(1);
    }
}

