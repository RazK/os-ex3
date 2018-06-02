//
// Created by razk on 02/06/18.
//
#include "Context.h"

Context::Context() : emit2Accumulator(new IntermediateVec) {}

Context::~Context() {
    delete this->emit2Accumulator;
}

void Context::append(const IntermediatePair pair) {
    this->emit2Accumulator->push_back(pair);
}