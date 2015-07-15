//
//  network.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/11/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#include "demand.h"
#include "facility.h"
#include "substrategraph.h"

using std::vector;

#define BANDWIDTH_UNIT    1
#define NO_SUPPLY         0
#define NO_FLOW           0
#define NO_FACILITY       false

typedef int Supply;
typedef int Flow;

namespace Network {
    
    void normalizeArcCap    (SubstrateGraph&, float);
    void normalizeNodeCap   (SubstrateGraph&, float);
    void addFootPrint       (SubstrateGraph&, SubstrateGraph::ArcCapacityMap&, Arc&, Capacity);
    void removeFootPrint    (SubstrateGraph&, SubstrateGraph::ArcCapacityMap&, Arc&, Capacity);
    void allocateArcRes     (SubstrateGraph&, Arc&, Capacity);
    void releaseArcRes      (SubstrateGraph&, Arc&, Capacity);
    void printArcs          (SubstrateGraph&);
    void printAllocatedArcs (SubstrateGraph&);
    void printNodes         (SubstrateGraph&);
    void printAllocatedNodes(SubstrateGraph&);
    void addFlow            (SubstrateGraph&, SubstrateGraph::ArcMap<Flow>&);
    void removeFlow         (SubstrateGraph&, SubstrateGraph::ArcMap<Flow>&);
    void printFlow          (SubstrateGraph&, SubstrateGraph::ArcMap<Flow>&);
    void printExistingFlow  (SubstrateGraph&, SubstrateGraph::ArcMap<Flow>&);
    void printFlow          (SubstrateGraph&, vector<Arc>&);
}
