//
// Created by razk on 02/06/18.
//
#include "Context.h"

Context::Context(int tid) : tid(tid), emit2Accumulator(new std::vector<IntermediatePair>){}

Context::~Context() {
    delete this->emit2Accumulator;
}