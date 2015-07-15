//
//  mapping.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/11/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#include <map>
#include <vector>
#include <unordered_map>
#include "demand.h"
#include "facility.h"
#include "substrategraph.h"
#include "network.h"
#include "graph.h"

using std::string;
using std::unordered_map;
using std::map;
using std::vector;

#define FACILITY_REQ_CAP 1

class Mapping {
#ifndef __SLFL_H__
    friend class SimpleLazyFacilityLocation;
#endif
public:
    //protected:
    unordered_map<Identifier, Identifier>   demands2FacsMap_;
    unordered_map<Identifier, Facility>     facilities_;
    unordered_map<Identifier, Demand>       demands_;
    unordered_map<Identifier, vector<Arc> > demandFlows_;
    
    void trackFlow_     (Node&  , Node&,     SubstrateGraph&, SubstrateGraph::FlowMap&, vector<Arc>&);
    void findFlowPath_  (Facility&, Demand&, SubstrateGraph&, SubstrateGraph::FlowMap&, vector<Arc>&);
    
//    void trackFlow_     (Node&  , SubstrateGraph&, SubstrateGraph::FlowMap&, vector<Arc>&);
//    void findFlowPath_  (Demand&, SubstrateGraph&, SubstrateGraph::FlowMap&, vector<Arc>&);
    
public:
    Mapping();
    Facility facility(Demand);
    int      facilityNum();
    int      demandNum  ();
    Demand   demand     (Identifier);
    void     demands    (Facility, vector<Demand>&);
    void     addDemand  (Demand);
    void     remove     (Demand);
    void     install    (Facility, SubstrateGraph&);
    void     remove     (Facility, SubstrateGraph&);
    void     assign     (Facility, Demand, SubstrateGraph&, vector<Arc>&);
    void     assign     (Facility, Demand, SubstrateGraph&, SubstrateGraph::FlowMap&);
    void     assign     (Facility, vector<Demand>&, SubstrateGraph&, SubstrateGraph::FlowMap&);
    void     deassign   (Demand, SubstrateGraph&);
    void     deassign   (vector<Demand>&, SubstrateGraph&);
//    void     reassign   (Demand, Facility, SubstrateGraph&, vector<Arc>&);
//    void     reassign   (Demand, Facility, SubstrateGraph&, SubstrateGraph::FlowMap&);
    
    void     removeNetworkFootprint(Demand  , SubstrateGraph&);
    void     removeNetworkFootprint(vector<Demand>&, SubstrateGraph&);
    void     removeNetworkFootprint(Facility, SubstrateGraph&);
    void     backNetworkFootprint  (Demand  , SubstrateGraph&);
    void     backNetworkFootprint  (vector<Demand>&, SubstrateGraph&);
    void     backNetworkFootprint  (Facility, SubstrateGraph&);
    
    void     migrate (Facility, Node, SubstrateGraph&, SubstrateGraph::FlowMap&);
    
    void     flow    (Demand  , vector<Arc>&);
    void     flow    (Demand  , SubstrateGraph::FlowMap&);
    void     flow    (vector<Demand>&, vector<Arc>&);
    void     flow    (Facility, unordered_map<Identifier, vector<Arc> >&);
    void     flow    (Facility, vector<Arc>&);
    void     flow    (Facility, SubstrateGraph::FlowMap&);
    Flow     flow    (Facility);
};