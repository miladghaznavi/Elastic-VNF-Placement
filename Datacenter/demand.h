//
//  demand.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 4/18/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once
#include "basics.h"
#include <queue>
#include <string>

using std::priority_queue;
using std::string;

#define DEFAULT_ARRIVAL_TIME    0
#define DEFAULT_DURATION        0

class Demand : public BaseClass {
#ifdef _EVENT_H_
friend class Event;
#endif
private:
    Identifier _src;
    Identifier _target;
    Identifier _kariz;
    double     _arrTime;
    double     _duration;

protected:
    static int demandsCount_;
    
    void out_ (std::ostream&);
    void in_  (std::istream&);
    void init_(Identifier, Identifier, Identifier, double, double);
    
public:
    Demand(Identifier = INVALID_ID, Identifier = INVALID_ID);
    Demand(Identifier, Identifier, double, double);
    
    Identifier source   ();
    Identifier target   ();
    double     arrival  ();
    double     duration ();
    double     departure();
    void       source   (Identifier);
    void       target   (Identifier);
    void       arrival  (double);
    void       duration (double);
    void       kariz    (Identifier);
    Identifier kariz    ();
    
    friend bool operator== (Demand&, Demand&);
    friend bool operator!= (Demand&, Demand&);
};

