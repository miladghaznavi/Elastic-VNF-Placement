//
//  demand.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 4/18/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "demand.h"
#include <stdexcept>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

int Demand::demandsCount_ = 0;

void Demand::init_(Identifier id, Identifier source, Identifier target, double arrival, double duration) {
    this->identifier(id);
    this->source(source);
    this->target(target);
    this->arrival(arrival);
    this->duration(duration);
}

Demand::Demand(Identifier source, Identifier target, double arrival, double duration) {
    init_(demandsCount_++, source, target, arrival, duration);
}

Demand::Demand(Identifier source, Identifier target) :
    Demand(source, target, DEFAULT_ARRIVAL_TIME, DEFAULT_DURATION) { }

void Demand::out_(std::ostream& os) {
    os << identifier() << " " << source() << " " << target() << " " << arrival() << " " << duration();
}

void Demand::in_ (std::istream& is) {
    Identifier id, source, target;
    double arrival, duration;
    is >> id >> source >> target >> arrival >> duration;
    init_(id, source, target, arrival, duration);
}

Identifier Demand::source() {
    return _src;
}

Identifier Demand::target() {
    return _target;
}

double Demand::arrival() {
    return _arrTime;
}

double Demand::duration() {
    return _duration;
}

double Demand::departure() {
    return _arrTime + _duration;
}

void Demand::source(Identifier source) {
    if (_src < 0) {
        std::stringstream ss;
        ss << "source is not valid. source is '" << source << "'!";
        throw std::invalid_argument(ss.str());
    }//if
    _src = source;
}

void Demand::target(Identifier target) {
    if (target < 0) {
        std::stringstream ss;
        ss << "target is not valid. target is '" << target << "'!";
        throw std::invalid_argument(ss.str());
    }//if
    _target = target;
}

void Demand::arrival(double arrival) {
    if (arrival < 0) {
        throw std::invalid_argument("Arrival time cannot be negative!");
    }//if
    this->_arrTime = arrival;
}

void Demand::duration(double duration) {
    if (duration < 0) {
        throw std::invalid_argument("Duration cannot be negative!");
    }//if
    this->_duration = duration;
}

void Demand::kariz(Identifier kariz) {
    if (kariz < 0) {
        std::stringstream ss;
        ss << "kariz is not valid. kariz is '" << kariz << "'!";
        throw std::invalid_argument(ss.str());
    }//if
    _kariz = kariz;
}

Identifier Demand::kariz() {
    return _kariz;
}

bool operator== (Demand &d1, Demand &d2) {
    return d1.identifier() == d2.identifier() &&
           d1._src         == d2._src         &&
           d1._target      == d2._target      &&
           d1._kariz       == d2._kariz       &&
           d1._arrTime     == d2._arrTime     &&
           d1._duration    == d2._duration;
}

bool operator!= (Demand &d1, Demand &d2) {
    return !(d1 == d2);
}