//
//  cost.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 5/28/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#include <vector>
#include <array>
#include "basics.h"
#include "flowgraph.h"
#include "fattree.h"

using std::vector;
using std::array;

#define NO_COST 0.0

typedef double Money;
typedef double Time;
typedef double Memory;

struct Parameters {
    Time     timeSlotInSecond;
    Money    hostCostInCentsPerTimeSlot;
    Money    bandwidthUnitCostInCentsPerTimeSlot;
    Capacity vnfMaxCapInTermsOfDemandsNum;
    Memory   vnfRamSizeInTermsOfBandwidthUnit;
};

class Cost {
public:
    typedef Dijkstra<FlowGraph, FlowGraph::EdgeWeight> ShortestPath;
    
private:
    Parameters _params;
    double**   _shortestPaths;
    
    void _findShortestPaths(FlowGraph&, KAryFatTree&);
    Edge _findEdge(Node, FlowGraph&, KAryFatTree&);
    Node _findHost(Node, FlowGraph&, KAryFatTree&);
    
public:
    Cost(Parameters, FlowGraph&, KAryFatTree&);
    
    Money f();
    Money g();
    Money k(Node, Node, FlowGraph&, KAryFatTree&);
    Money h(Node, Node, FlowGraph&, KAryFatTree&);
};