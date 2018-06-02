//
// Created by razk on 02/06/18.
//
#include "Context.h"

Context::Context() : emit2Accumulator(new IntermediateVec){}

Context::~Context() {
    delete this->emit2Accumulator;
}