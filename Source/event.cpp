//
//  event.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/5/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "event.h"
#include <stdlib.h>
#include <sstream>
#include <istream>
#include <unistd.h>
#include <iostream>

bool Event::_compare(const Event& e1, const Event& e2) {
    double t1 = (e1.type == Event::Arrival) ? e1.demand._arrTime : e1.demand._arrTime + e1.demand._duration;
    double t2 = (e2.type == Event::Arrival) ? e2.demand._arrTime : e2.demand._arrTime + e2.demand._duration;
    return t2 < t1;
}

Event::Event(Demand& d, Type t) : demand(d), type(t) { }

bool operator < (const Event& e1, const Event& e2) {
    return Event::_compare(e1, e2);
}

void EventQueue::init(string path) {
    if (access(path.c_str(), F_OK) != -1) {
        _path = path;
    }// if
    else {
        std::stringstream ss;
        ss << "No file '" << path << "' exists!";
        throw std::invalid_argument(ss.str());
    }//else
}

size_t EventQueue::parse() {
    std::ifstream inputFile(_path.c_str());
    
    Identifier src, trg;
    double     arr, dur;
    Identifier id = 0;
    
    while (!inputFile.eof()) {
        // Read demand from file
        inputFile >> src >> trg >> arr >> dur;
        Demand d(src, trg, arr, dur);
        d.identifier(id++);
        
        // Arrival event
        Event arrival  (d, Event::Arrival  );
        arrival.identifier(d.identifier());
        
        // Departure event
        Event departure(d, Event::Departure);
        departure.identifier(d.identifier());
        
        // Push arrival and departure
        _queue.push(arrival);
        _queue.push(departure);
    }//while
    
    return _queue.size();
}

bool EventQueue::empty() {
    return _queue.empty();
}

Event EventQueue::next() {
    Event e = _queue.top();;
    _queue.pop();
    return e;
}

Event EventQueue::top() {
    return _queue.top();
}

EventQueue::~EventQueue() { }
