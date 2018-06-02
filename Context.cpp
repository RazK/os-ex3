//
// Created by razk on 02/06/18.
//
#include "Context.h"
#include <algorithm>    // std::sort

Context::Context(Context(std::atomic<bool> const & suffleLocked, Barrier const & shuffleBarrier)) :
        shuffleLocked(shuffleLocked),
        shuffleBarrier(shuffleBarrier),
        emit2Accumulator(new IntermediateVec()) {}

Context::~Context() {
    delete this->emit2Accumulator;
}

void Context::sort() {

//    std::sort(this->emit2Accumulator.begin(), this->emit2Accumulator.end());
//    std::sort(std::vector<int>)

}


void Context::append(const IntermediatePair& pair) {
    this->emit2Accumulator->push_back(pair);
}