//
//  resource.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 5/1/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "resource.h"
#include <stdexcept>
#include <sstream>

ResourceBase::ResourceBase(int capacity) {
    capacity_(capacity);
}

int ResourceBase::capacity() {
    return _capacity;
}

void ResourceBase::capacity_(int capacity) {
    if (capacity < 0) {
        throw std::invalid_argument("Capacity cannot be negative!");
    }//if
    this->_capacity = capacity;
}

void ResourceBase::init_(int capacity, int allocated) {
    this->capacity_(capacity);
    this->_allocated = DEFAULT_INIT_ALLOC;
    this->allocate(allocated);
}

int ResourceBase::residualCapacity() {
    return _capacity - _allocated;
}

int ResourceBase::allocated() {
    return _allocated;
}

void ResourceBase::allocate(int allocate) {
    if (residualCapacity() < allocate) {
        std::stringstream ss;
        ss  << "Cannot allocate more than residual capacity. Requested allocation is '"
            << allocate << "' and residual capacity is '" << residualCapacity() << "'!";
        throw std::invalid_argument(ss.str());
    }//if
    _allocated += allocate;
}

void ResourceBase::release (int release) {
    if (_allocated < release) {
        std::stringstream ss;
        ss  << "Cannot release more than what is allocated. Requested release is '"
            << release << "' and allocated capacity is '" << _allocated << "'!";
        throw std::invalid_argument(ss.str());
    }//if
    _allocated -= release;
}

void ResourceBase::releaseAll() {
    release(allocated());
}

ResourceBase::operator int () {
    return this->residualCapacity();
}