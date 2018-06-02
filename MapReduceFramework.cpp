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
    return;
}

void runMapReduceFramework(const MapReduceClient& client,
                           const InputVec& inputVec, OutputVec& outputVec,
                           int multiThreadLevel){
    FrameWork framework = FrameWork(client, inputVec, outputVec, multiThreadLevel);
    framework.run();


}

