//
//  event.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/5/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#ifndef _EVENT_H_
#define _EVENT_H_
#endif

#include "demand.h"
#include "cost.h"

class Event : public BaseClass {
private:
    static bool _compare(const Event&, const Event&);

public:
    enum Type {
        Arrival,
        Departure
    };

    Type   type;
    Demand demand;
    
    Event(Demand&, Type);
    
    friend bool operator < (const Event&, const Event&);
};

class EventQueue {
private:
    priority_queue<Event> _queue;
    string                _path;
    
public:
    void init(string);
    
    size_t parse();
    bool   empty();
    Event  next ();
    // top returns the next event without removing it
    Event  top  ();
    
    ~EventQueue();
};
