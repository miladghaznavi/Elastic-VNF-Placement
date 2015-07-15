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
#include "network.h"
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
    float    bandwidthUnitInMB;
    Money    bandwidthUnitCostInCentsPerTimeSlot;
    Capacity vnfCPU;
    Capacity vnfMaxCapInTermsOfDemandsNum;
    Memory   vnfRamSizeInTermsOfBandwidthUnit;
};

class Cost {
public:
    typedef Dijkstra<SubstrateGraph, SubstrateGraph::EdgeWeightMap> ShortestPath;
    
private:
    Parameters _params;
    double**   _shortestPaths;
    
    void _findShortestPaths(SubstrateGraph&, KAryFatTree&);
    Arc  _findArc          (Node&, SubstrateGraph&, KAryFatTree&);
    
public:
    Cost(Parameters&, SubstrateGraph&, KAryFatTree&);
    
    Money f();
    Money g();
    Money k(Node, Node, SubstrateGraph&, KAryFatTree&);
    Money h(Node, Node, SubstrateGraph&, KAryFatTree&);
};