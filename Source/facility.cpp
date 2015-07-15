//
//  facility.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 4/18/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "facility.h"
#include <sstream>
#include <stdexcept>

int Facility::facilitiesCount_ = 0;

void Facility::init_(Identifier id, Identifier location, int capacity, int allocated) {
    this->identifier(id);
    this->location(location);
    ResourceBase::init_(capacity, allocated);
}

Facility::Facility(Identifier location, int capacity) {
    init_(facilitiesCount_ ++, location, capacity, DEFAULT_INIT_ALLOC);
}

Facility::Facility(int capacity) : Facility(DEFAULT_LOCATION, capacity) { }

void Facility::in_ (std::istream& is) {
    Identifier id, location;
    int cap;
    is >> id >> location >> cap;
    init_(id, location, cap, DEFAULT_INIT_ALLOC);
}

void Facility::out_(std::ostream& os) {
    os << identifier() << " " << location() << " " << capacity();
}

Identifier Facility::location() {
    return _location;
}

void Facility::location(Identifier node) {
    this->_location = node;
}
