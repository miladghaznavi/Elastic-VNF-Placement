//
//  randalg.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/6/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#include "algorithm.h"

class RandAlgorithm: public Algorithm {
private:
    bool _assignment(Demand& d, Facility&, SubstrateGraph::FlowMap&);
    bool _install   (Demand& d, Node&, SubstrateGraph::FlowMap&);
    
public:
    static const string NAME;
    
    RandAlgorithm(Parameters, SubstrateGraph&, KAryFatTree&);
    
    Money  reassignmentCost();
    size_t reassignmentNum ();
    Money  migrationCost   ();
    size_t migrationNum    ();
    
    bool   arrival  (Demand, double);
    void   departure(Demand, double);
    string name     ();
};